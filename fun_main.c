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
	printf("In questo sistema potrai eseguire diverse operazioni su file e directory\ncon i seguenti comandi:\n");
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
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			free(array_fat[i]);
		}
	}
	//RICEZIONE DELL'ARRAY DI ELEMENTI
	int size=sizeof(struct fat)*MAX_INODE;
	int id=shmget(12345, size, IPC_CREAT | 0666);
	if(id==-1) handle_error("Error shmid\n");
	struct fat* shmem;
	shmem=(struct fat*)shmat(id, 0, 0);
	for(int i=0; i<MAX_INODE; i++){
			if(shmem[i].inode==INODE_LIMIT){
				array_fat[i]=NULL;
			}else{
				array_fat[i]=(struct fat*)calloc(1, sizeof(struct fat));
				array_fat[i]->inode=shmem[i].inode;
				array_fat[i]->inode_padre=shmem[i].inode_padre;
				int l=strlen(shmem[i].name);
				for(int j=0; j<l; j++){
					array_fat[i]->name[j]=shmem[i].name[j];
				}
				l=strlen(shmem[i].path);
				for(int j=0; j<l; j++){
					array_fat[i]->path[j]=shmem[i].path[j];
				}
				l=strlen(shmem[i].type);
				for(int j=0; j<l; j++){
					array_fat[i]->type[j]=shmem[i].type[j];
				}
				l=strlen(shmem[i].creator);
				for(int j=0; j<l; j++){
					array_fat[i]->creator[j]=shmem[i].creator[j];
				}
			}
	}
	shmdt((void *)shmem);
}

void sendToServer(char* elem){
	int ret;
	int size_elem=strlen(elem);
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
	int ret;
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
	printf("%s\n", res);
	ret=close(fdfifo);
	if(ret==-1) handle_error("Errore: impossibile chiudere il canale di comunicazione\n");
}

void stampaArray(){  //DA CANCELLARE
	printf("Stampa array:\n");
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			printf("Inode %d, creator %s, nome %s,\npercorso %s, tipo %s, inode padre %d\n\n", array_fat[i]->inode, array_fat[i]->creator, array_fat[i]->name, array_fat[i]->path, array_fat[i]->type, array_fat[i]->inode_padre);
		}
	}
	printf("Stampato\n");
}

void currentPath(){
	if(current_path!=NULL){
		free(current_path);
	}
	if(memcmp(fat_padre->path, "/", 1)==0){
		current_path=(char*)calloc(strlen(fat_padre->name)+3, sizeof(char));
		int j=0;
		int k=0;
		int e=0;
		while(!e){
			if(fat_padre->name[j]){
				current_path[k]=fat_padre->name[j];
				j++;
			}else{
				current_path[k]='/';
				e=1;
			}
			k++;
		}
	}else{
		int size=strlen(fat_padre->path)+strlen(fat_padre->name)+3;
		current_path=(char*)calloc(size, sizeof(char));
		int k=0;
		int i=0;
		int j=0;
		int e=0;
		while(!e){
			if(fat_padre->path[i]){
				current_path[k]=fat_padre->path[i];
				i++;
			}else if(fat_padre->name[j]){
				current_path[k]=fat_padre->name[j];
				j++;
			}else{
				current_path[k]='/';
				e=1;
			}
			k++;
		}
	}
	printf("Percorso corrente: ");
	int i=0;
	while(current_path[i]){
		printf("%c", current_path[i]);
		i++;
	}
	printf("\n");
}

int searchElement(char* name, char* path){
	int trovato=0;
	int inode;
	for(int i=0; i<MAX_INODE && !trovato; i++){
		if(array_fat[i]!=NULL){
			if(strcmp(array_fat[i]->name, name)==0 && strcmp(array_fat[i]->path, path)==0){
				inode=i;
				trovato=1;
			}
		}
	}
	if(trovato){
		return inode;
	}else{
		return 0;
	}
}

void copyArrayString(char elem[64], char copy[64]){
	int k=0;
	while(copy[k]){
		elem[k]=copy[k];
		k++;
	}
}

void getCmd(char dest[50], char src[50]){
	int stop=0;
	for(int i=0; i<50 && !stop; i++){
		if(src[i]!=' ' && src[i]!='\n'){
			dest[i]=src[i];
		}else{
			stop=1;
		}
	}
}

int compareArrayString(char elem[64], char copy[64]){
	int ret=1;
	for(int i=0; i<64 && ret; i++){
		if(elem[i]==copy[i]){
			ret=0;
		}
	}
	return ret;
}

int isNumber(char src[64]){
	int ret=1;
	int len=strlen(src);
	for(int i=0; i<len && ret; i++){
		if((src[i]<48 || src[i]>57) && src[i]>10){
			ret=0;
		}
	}
	return ret;
}
