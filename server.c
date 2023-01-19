#include "var.h"
#include "fun_server.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int main(){
	array_fat=(struct fat**)calloc(MAX_INODE, sizeof(struct fat*));
	inode_fat=0;
	inode_dir=1;
	sem_unlink(SEM_SERVER);
	server=sem_open(SEM_SERVER, O_CREAT | O_EXCL, 0666, 1);
	if(server==SEM_FAILED) handle_error("Errore: impossibile avviare sem_server\n");
	sem_unlink(SEM_MAIN);
	main_s=NULL;
	main_s=sem_open(SEM_MAIN, O_CREAT | O_EXCL, 0666, 0);
	if(main_s==SEM_FAILED) handle_error("Errore: impossibile avviare sem_main\n");
	sem_unlink(SEM_SHMEM);
	shmem=NULL;
	shmem=sem_open(SEM_SHMEM, O_CREAT | O_EXCL, 0666, 1);
	if(shmem==SEM_FAILED) handle_error("Errore: impossibile avviare sem_shmem\n");
	int ret;
	current_path="/";
	FILE* exist=fopen(FAT_FILE_NAME, "r");
	if(exist==NULL){
		createFile(0, FAT_FILE_NAME, FILE_TYPE, GENERIC_CREATOR, "/", SERVER_FATHER);
		fdfat=open(FAT_FILE_NAME, O_RDWR);
		if(fdfat==-1) handle_error("Errore: impossibile aprire il FAT.txt\n");
		for(int i=0; i<MAX_INODE; i++){
			char buffer[100];
			sprintf(buffer, "%d\n", i);
			write(fdfat, buffer, strlen(buffer));
		}
		ret=close(fdfat);
		if(ret==-1) handle_error("Errore: impossibile chiudere il FAT.txt\n");
		//INSERIRE FILE E DIRECTORY SUL FAT.txt
		char row[100];
		sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, FAT_FILE_NAME, current_path, FILE_TYPE, GENERIC_CREATOR, 0, SERVER_FATHER);
		insertInFatFile(row, next_inode);
		nextInode();
		createDirectory(next_inode, FILE_SYSTEM_DIRECTORY, DIR_TYPE, GENERIC_CREATOR, "/", SERVER_FATHER);
		//INSERIRE FILE E DIRECTORY SUL FAT.txt
		sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, FILE_SYSTEM_DIRECTORY, current_path, DIR_TYPE, GENERIC_CREATOR, 0, SERVER_FATHER);
		insertInFatFile(row, next_inode);
	}else{
		//CARICARE LA STRUTTURA CHE TIENE MEMORIA DEI FILE E DELLE DIRECTORY CREATE
		loadFAT();
		fclose(exist);
	}
	stampaArray();
	nextInode();
	sharing_father();
	printf("Server pronto all'avvio\n");
	current_path=*FILE_SYSTEM_DIRECTORY+"/";
	unlink(FIFO_FOR_FAT);
	int loop=1;
	int send=0;
	while(loop){
		printf("In attesa dei comandi degli utenti...\n");
		ret=mkfifo(FIFO_FOR_FAT, 0666);
		if(ret==-1) handle_error("Errore: impossibile creare la fifo per ricevere istruzioni dagli utenti\n");
		int fdfifo=open(FIFO_FOR_FAT, O_RDONLY);
		if(fdfifo==-1) handle_error("Errore: impossibile aprire la fifo per comunicare con gli utenti\n");
		int bytes_read=0;
		while(1){
			ret=read(fdfifo, buf+bytes_read, 100);
			if(bytes_read==-1){
				if(errno==EINTR){
					continue;
				}else{
					handle_error("Errore: impossibile leggere il messaggio dell'utente\n");
				}
			}else if(ret==0){
				break;
			}else{
				bytes_read+=ret;
			}
		}
		printf("\nMessaggio ricevuto %s\n", buf);
		ret=close(fdfifo);
		if(ret==-1) handle_error("Errore: impossibile chiudere il descrittore della fifo\n");
		ret=unlink(FIFO_FOR_FAT);
		if(ret==-1) handle_error("Errore: impossibile eliminare la fifo\n");
		char* elem=strtok(buf, SEPARATOR);
		printf("Primo elemento ricevuto %s\n", elem);
		if(strcmp(elem, CREAT_CMD)==0){
			//CREAZIONE
			printf("GESTIONE CREAZIONE DELLA RIGA\n");
			char* nome=strtok(NULL, SEPARATOR);
			char* type=strtok(NULL, SEPARATOR);
			char* creator=strtok(NULL, SEPARATOR);
			char* path=strtok(NULL, SEPARATOR);
			int inode_padre=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			if(strcmp(DIR_TYPE, type)==0){
				createDirectory(next_inode, nome, type, creator, path, inode_padre);
			}else{
				createFile(next_inode, nome, type, creator, path, inode_padre);
			}
			send=1;
			nextInode();
		}else if(strcmp(elem, DELETE_CMD)==0){
			//GESTIONE ELIMINAZIONE DELLA RIGA
			printf("GESTIONE ELIMINAZIONE DELLA RIGA\n");
			char* nome=strtok(NULL, SEPARATOR);
			char* type=strtok(NULL, SEPARATOR);
			char* path=strtok(NULL, SEPARATOR);
			int inode=searchElement(nome, path, *type);
			if(inode){
				if(strcmp(type, DIR_TYPE)==0){
					printf("Cancello direcotry...\n");
					eraseDirectory(inode);
				}else{
					printf("Cancello file...\n");
					eraseFile(inode);
				}
				sendResult("Cancellazione avvenuta con successo");
				nextInode(fdfat);
			}else{
				sendResult("Impossibile eliminare il file o la directory");
			}
			send=1;
		}else if(strcmp(elem, UPDATE_CMD)==0){
			//GESTIONE AGGIORNAMENTO DIMESIONE DI UNA RIGA DEL FILE FAT.txt
			int inode=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			insertInFatFile(buf, inode);
		}else if(strcmp(elem, CLOSE_CMD)==0){
			printf("Chiusura in corso...\n");
			loop=0;
		}
		if(send){
			send=0;
			ret=sem_wait(server);
			if(ret==-1) handle_error("Errore: sem_wait server\n");
			ret=sem_wait(shmem);
			if(ret==-1) handle_error("Errore: sem_wait shmem\n");
			sharing_father();
			ret=sem_post(shmem);
			if(ret==-1) handle_error("Errore: sem_post shmem\n");
			ret=sem_post(main_s);
			if(ret==-1) handle_error("Errore: sem_post main	\n");
		}
		memset(buf, 0, 100);
		stampaArray();
	}
	ret=sem_close(server);
	if(ret==-1) handle_error("Errore: sem_close server\n");
	ret=sem_close(shmem);
	if(ret==-1) handle_error("Errore: sem_close shmem\n");
	ret=sem_close(main_s);
	if(ret==-1) handle_error("Errore: sem_close main\n");
	ret=sem_unlink(SEM_SERVER);
	if(ret==-1) handle_error("Errore: sem_destroy server\n");
	ret=sem_unlink(SEM_MAIN);
	if(ret==-1) handle_error("Errore: sem_destroy shmem\n");
	ret=sem_unlink(SEM_SHMEM);
	if(ret==-1) handle_error("Errore: sem_destroy main\n");
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			free(array_fat[i]);
		}
	}
	free(array_fat);
}
