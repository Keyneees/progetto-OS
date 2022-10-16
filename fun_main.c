#include "fun_main.h"
#include "var.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void printInfo(){
	printf("In questo sistema potrai eseguire diverse operazioni su file e directory con i seguenti comandi:\n");
	printf("\tcf -> comando per creare un nuovo file\n");
	printf("\tef -> comando per eliminare un file\n");
	printf("\twf -> comando per scrivere in un file\n");
	printf("\trf -> comando per leggere il contenuto di un file\n");
	printf("\tsf -> comando per spostare il cursore dentro al file\n");
	printf("\tcd -> comando per creare una nuova directory\n");
	printf("\ted -> comando per eliminare una directory\n");
	printf("\tmd -> comando per cambiare directory corrente\n");
	printf("\tld -> comando per stampare gli elementi presenti nella direcoty corrente\n");
	printf("\thelp -> comando per visualizzare i comandi eseguibili nel sistema\n");
	printf("\texit -> comando per chiudere il sistema\n");
	printf("\tclose -> comando per chiudere sia il sistema sia il server\n");
}

void printInfoMD(){
	printf("Nel dettaglio, il comando 'md' dovrà essere seguito da:\n");
	printf("\tnome della direcotry -> se l'utente si vorrà spostare in una \n\tdelle direcotry presenti in quella corrente\n");
	printf("\t.. -> se l'utente vorrà spostarsi nella direcotry in cui è contenuta la \n\tdirectory corrente\n");
	printf("NB: non è possibile spostarsi in una directory che non esiste in quella corrente e non è possibile spostarsi nella directory padre di '%s'\n", FILE_SYSTEM_DIRECTORY);
}

void sharing_father(){
	//RICEZIONE DI UN SINGOLO ELEMENTO
	/*fat_padre=(struct fat*)malloc(sizeof(struct fat));
	int fd=shm_open(SHMEM_FOR_INFO, O_RDONLY, 0666);
	if(fd==-1) handle_error("Errore: impossibile aprire la memoria condivisa\n");
	int size=sizeof(struct fat)+sizeof(char)*256;
	fat_padre=(struct fat*)mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
	if(fat_padre==MAP_FAILED) handle_error("Errore: impossibile recuperare dati dal memoria condivisa\n");
	int ret=close(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere la memoria condivisa\n");*/
	
	//RICEZIONE DELL'ARRAY DI ELEMENTI
	int size=sizeof(struct fat)*MAX_INODE;
	int id=shmget(12345, size, IPC_CREAT | 0666);
	if(id==-1) handle_error("Error shmid\n");
	struct fat* shmem;
	shmem=(struct fat*)shmat(id, 0, 0);
	//printf("Shmem:\n");
	for(int i=0; i<MAX_INODE; i++){
			//printf("Inode %d, size %d, creator %s, nome %s, percorso %s, tipo %s, inode_padre %d\n", shmem[i].inode, shmem[i].size, shmem[i].creator,
			//shmem[i].name, shmem[i].path, shmem[i].type, shmem[i].inode_padre);
			if(shmem[i].inode==INODE_LIMIT){
				//array_fat[i]=(struct fat*)malloc(sizeof(struct fat));
				array_fat[i]=NULL;
			}else{
				array_fat[i]=(struct fat*)malloc(sizeof(struct fat));
				array_fat[i]->inode=shmem[i].inode;
				array_fat[i]->size=shmem[i].size;
				array_fat[i]->inode_padre=shmem[i].inode_padre;
				strcpy(array_fat[i]->name, shmem[i].name);
				strcpy(array_fat[i]->path, shmem[i].path);
				strcpy(array_fat[i]->type, shmem[i].type);
				strcpy(array_fat[i]->creator, shmem[i].creator);
			}
	}
	shmdt((void *)shmem);
	//shmctl(id, IPC_RMID, NULL);
	//printf("Fine shmem\n");
	
}

void sendToServer(char* elem){
	int ret;
	int size_elem=strlen(elem);
	//printf("Mi preparo per aggiungere elementi nella fat\n");
	int fdfifo=open(FIFO_FOR_FAT, O_WRONLY);
	if(fdfifo==-1) handle_error("Errore: impossibile connetersi alla fifo di comunicazione con il server\n");			
	int send_bytes=0;
	while(send_bytes<size_elem){
		ret=write(fdfifo, elem+send_bytes, size_elem-send_bytes);
		if(ret==-1){
			if(errno==EINTR){
				continue;
			}else{
				handle_error("Errore: impossibile mandare il messaggio al server\n");
			}
		}else{
			send_bytes+=ret;
		}
	}
	ret=close(fdfifo);
	if(ret==-1) handle_error("Errore: impossibile chiudere la fifo di comunicazione con il server\n");
	if(strcmp(CLOSE_CMD, elem)!=0){
		waitResult();
	}
}

void waitResult(){
	int ret=mkfifo(FIFO_FOR_RES, 0666);
	if(ret==-1) handle_error("Errore: impossibile creare la fifo per ricevere istruzioni dagli utenti\n");	
	int fdfifo=open(FIFO_FOR_RES, O_RDONLY);
	if(fdfifo==-1) handle_error("Errore: impossibile aprire la fifo per comunicare con gli utenti\n");
	int bytes_read=0;
	char res[128];
	memset(res, 0, 128);
	printf("In attesa del server...\n");
	while(1){
		ret=read(fdfifo, res+bytes_read, 100);
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
	//fflush(stdout);
	printf("%s\n", res);
	ret=close(fdfifo);
	if(ret==-1) handle_error("Errore: impossibile chiudere il canale di comunicazione\n");
	ret=unlink(FIFO_FOR_RES);
	if(ret==-1) handle_error("Errore: impossibile eliminare il canale di comunicazione\n");
}

void stampaArray(){  //DA CANCELLARE
	printf("Stampa array:\n");
	for(int i=0; i<MAX_INODE; i++){
		//printf("%d NULL? %d\n", i, array_fat[i]==NULL);
		if(array_fat[i]!=NULL){
			printf("Inode %d, size %d, creator %s, nome %s,\npercorso %s, tipo %s, inode padre %d\n\n", array_fat[i]->inode, array_fat[i]->size, array_fat[i]->creator, array_fat[i]->name, array_fat[i]->path, array_fat[i]->type, array_fat[i]->inode_padre);
		}else{
			//printf("Inode %d, NULL\n", i);
		}
	}
	printf("Stampato\n");
}

void currentPath(){
	if(strcmp(fat_padre->path, "/")==0){
		printf("Percorso trovato: /\n");
		current_path=(char*)malloc(sizeof(char)*(strlen(fat_padre->name)+3));
		current_path=strcpy(current_path, fat_padre->name);
		current_path=strcat(current_path, "/");
	}else{
		printf("Percorso non basilare\n");
		current_path=(char*)malloc(sizeof(char)*(strlen(fat_padre->path)+strlen(fat_padre->name)+3));
		current_path=strcpy(current_path, fat_padre->path);
		current_path=strcat(current_path, fat_padre->name);
		current_path=strcat(current_path, "/");
	}
}

int searchElement(char* name, char* path){
	printf("Ricerca dell'elemento...\n");
	int trovato=0;
	int inode;
	printf("Nome: %s\nPath: %s\n", name, path);
	for(int i=0; i<MAX_INODE && !trovato; i++){
		if(array_fat[i]!=NULL){
			if(strcmp(array_fat[i]->name, name)==0 && strcmp(array_fat[i]->path, path)==0){
				printf("Elemento trovato\n");
				inode=i;
				trovato=1;
			}else{
				printf("%d: Elemento con nome o percorso diverso\n", i);
			}
		}else{
			printf("%d: Elemento assente\n", i);
		}
	}
	if(trovato){
		printf("Elemento trovato\n");
		return inode;
	}else{
		printf("Elemento non trovato\n");
		return 0;
	}
}
