#include "fun.h"
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

int inode_fat=0;
int inode_dir=1;
int fdfat;
char buf[100];

int main(){
	int ret;
	current_path="/";
	FILE* exist=fopen(FAT_FILE_NAME, "r");
	if(exist==NULL){
		printf("Il file non esiste\n");
		createFile(0, FAT_FILE_NAME, FILE_TYPE, GENERIC_CREATOR, "/", SERVER_FATHER);
		
		fdfat=open(FAT_FILE_NAME, O_RDWR);
		if(fdfat==-1) handle_error("Errore: impossibile aprire il FAT.txt\n");
		for(int i=0; i<MAX_INODE; i++){
			char buffer[100];
			sprintf(buffer, "%d\n", i);
			write(fdfat, buffer, strlen(buffer));
		}
		//inode_fat=next_inode;
		ret=close(fdfat);
		if(ret==-1) handle_error("Errore: impossibile chiudere il FAT.txt\n");
		//INSERIRE FILE E DIRECTORY SUL FAT.txt
		char row[100];
		sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, FAT_FILE_NAME, current_path, FILE_TYPE, GENERIC_CREATOR, 0, SERVER_FATHER);
		printf("row calcolata %s", row);
		insertInFatFile(row, next_inode);
		
		nextInode();
		createDirectory(next_inode, FILE_SYSTEM_DIRECTORY, DIR_TYPE, GENERIC_CREATOR, "/", SERVER_FATHER);
		//INSERIRE FILE E DIRECTORY SUL FAT.txt
		sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, FILE_SYSTEM_DIRECTORY, current_path, DIR_TYPE, GENERIC_CREATOR, 0, SERVER_FATHER);
		insertInFatFile(row, next_inode);
		//inode_dir=next_inode;
		sizeUpdate(inode_fat);
		//nextInode();
	}else{
		printf("Il file esiste\n");
		//CARICARE LA STRUTTURA CHE TIENE MEMORIA DEI FILE E DELLE DIRECTORY CREATE
		loadFAT();
	}
	stampaArray();
	nextInode();
	printf("Array stampato\n");
	//printf("Caricamento inode padre...");
	sharing_father();
	printf(" Effettuato\n");
	printf("Server pronto all'avvio\n");
	current_path=*FILE_SYSTEM_DIRECTORY+"/";
	unlink(FIFO_FOR_FAT);
	int loop=1;
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
		printf("Messaggio ricevuto %s\n", buf);
		ret=close(fdfifo);
		if(ret==-1) handle_error("Errore: impossibile chiudere il descrittore della fifo\n");
		ret=unlink(FIFO_FOR_FAT);
		if(ret==-1) handle_error("Errore: impossibile eliminare la fifo\n");
		
		char* elem=strtok(buf, SEPARATOR);
		printf("Primo elemento ricevuto %s\n", elem);
		
		if(strcmp(elem, CREAT_CMD)==0){
			//CREAZIONE
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
			sizeUpdate(inode_fat);
			sizeUpdate(inode_dir);
			nextInode();
		}else if(strcmp(elem, DELETE_CMD)==0){
			//GESTIONE ELIMINAZIONE DELLA RIGA
			printf("GESTIONE ELIMINAZIONE DELLA RIGA\n");
			char* nome=strtok(NULL, SEPARATOR);
			char* type=strtok(NULL, SEPARATOR);
			char* path=strtok(NULL, SEPARATOR);
			int inode=searchElement(nome, path);
			if(inode){
				if(strcmp(type, DIR_TYPE)==0){
					printf("Cancello direcotry...\n");
					eraseDirectory(inode);
				}else{
					printf("Cancello file...\n");
					eraseFile(inode);
				}
				sendResult("Cancellazione avvenuta con successo\n");
				char in[10];
				sprintf(in, "%d\n", inode);
				insertInFatFile(in, inode);
				sizeUpdate(inode_fat);
				nextInode(fdfat);
			}else{
				sendResult("File o directory non esiste\n");
			}
		}else if(strcmp(elem, UPDATE_CMD)==0){
			//GESTIONE AGGIORNAMENTO DIMESIONE DI UNA RIGA DEL FILE FAT.txt
			int inode=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			insertInFatFile(buf, inode);
		}else if(strcmp(elem, CLOSE_CMD)==0){
			printf("Chiusura in corso...\n");
			loop=0;
		}else{
			//GESTIONE INSERIMENTO DELLA RIGA
			int inode=strtol(elem, NULL, 10);
			insertInFatFile(buf, inode);
			sizeUpdate(inode_fat);
			nextInode(fdfat);
		}
		memset(buf, 0, 100);
		stampaArray();
	}
	ret=shm_unlink(SHMEM_FOR_INFO);
	if(ret==-1) handle_error("Errore: impossibile fare l'unlink della memoria condivisa\n");
}
