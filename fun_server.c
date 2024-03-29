#include "fun_server.h"
#include "var.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


//FUNZONI PER I FILE
void createFile(int inode, char* filename, char* type, char* creator, char* path, int inode_padre){
	char res[128]={0};
	memset(res, 0, 128);
	if(next_inode<MAX_INODE){
		char* location=NULL;
		if(strcmp(path, "/")==0){
			location=(char*)malloc(sizeof(char)*(strlen(filename)));
			strcpy(location, filename);
		}else{
			location=(char*)malloc(sizeof(char)*(strlen(path)+strlen(filename)+2));
			strcpy(location, path);
			strcat(location, filename);
		}
		int create=open(location, O_CREAT | O_EXCL, PERMESSI_FILE);
		free(location);
		if(create==-1){
			if(errno==EEXIST){
				sprintf(res,"Errore: impossibile creare il file: nome già in uso");
				printf("%s", res);
			}else{
				sprintf(res, "Errore: impossbile creare il file");
				printf("%s", res);
			}
		}else{
			printf("File %s creato con successo\n", filename);
			printf("Creazione della struttura\n");
			array_fat[next_inode]=(struct fat*)calloc(1, sizeof(struct fat));
			array_fat[next_inode]->inode=next_inode;
			strcpy(array_fat[next_inode]->name, filename);
			strcpy(array_fat[next_inode]->path, path);
			strcpy(array_fat[next_inode]->type, type);
			strcpy(array_fat[next_inode]->creator, creator);
			array_fat[next_inode]->inode_padre=inode_padre;
			sprintf(res, "Elemento creato con successo");
			printf("%s", res);
			char row[100]={0};
			sprintf(row, "%d %s %s %s %s %d\n", next_inode, filename, path, type, creator, inode_padre);
			insertInFatFile(row, next_inode);
			printf("Fat file aggiornato\n");
		}
	}else{
		//GESTIONE IMPOSSIBLITA' DI CREARE IL FILE
		sprintf(res, "Impossibile creare il nuovo file, limite di file disponibili raggiunto");
		printf("%s", res);
	}
	printf("Fine preparazione fat_element\n");
	int dif=strcmp(GENERIC_CREATOR, creator);
	if(dif){
		printf("Messaggio mandato al server: %s\n", res);
		sendResult(res);
	}
}

void eraseFile(int inode){
	char* filename=(char*)malloc(sizeof(char)*(strlen(array_fat[inode]->path)+strlen(array_fat[inode]->name)+2));
	strcpy(filename, array_fat[inode]->path);
	strcat(filename, array_fat[inode]->name);
	int ret=remove(filename);
	free(filename);
	if(ret==-1){
		printf("Il file %s non esiste nella directory corrente\n", filename);
	}else{
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		char in[10]={0};
		sprintf(in, "%d\n", inode);
		insertInFatFile(in, inode);
	}
	
}

//FUNZIONI PER LE DIRECTORY
void createDirectory(int inode, char* directoryname, char* type, char* creator, char* path, int inode_padre){
	char res[128]={0};
	if(next_inode<MAX_INODE){
		char* location=NULL;
		if(strcmp(path, "/")==0){
			location=(char*)calloc((strlen(directoryname)), sizeof(char));
			strcpy(location, directoryname);
		}else{
			location=(char*)calloc((strlen(path)+strlen(directoryname)+2),sizeof(char));
			strcpy(location, path);
			strcat(location, directoryname);
		}
		int fddir=mkdir(location, PERMESSI_DIRECTORY);
		free(location);
		if(fddir==-1){
			if(errno==EEXIST){
				sprintf(res,"Errore: impossibile creare la directory: nome già in uso");
				printf("%s", res);
			}else{
				sprintf(res, "Errore: impossbile creare la directory");
				printf("%s", res);
			}
		}else{
			printf("File %s creato con successo\n", directoryname);
			printf("Creazione della struttura\n");
			array_fat[next_inode]=(struct fat*)calloc(1, sizeof(struct fat));
			array_fat[next_inode]->inode=inode;
			strcpy(array_fat[next_inode]->name, directoryname);
			strcpy(array_fat[next_inode]->path, path);
			strcpy(array_fat[next_inode]->type, type);
			strcpy(array_fat[next_inode]->creator, creator);
			array_fat[next_inode]->inode_padre=inode_padre;
			sprintf(res, "Elemento creato con successo");
			char row[100]={0};
			sprintf(row, "%d %s %s %s %s %d\n", next_inode, directoryname, path, type, creator, inode_padre);
			insertInFatFile(row, next_inode);
			printf("Fat file aggiornato\n");
		}
	}else{
		//GESTIONE IMPOSSIBILITA' DI CREARE LA DIRECTORY
		sprintf(res, "Impossibile creare la nuova directory, limite di file disponibili raggiunto");
		printf("%s", res);
		
	}
	if(strcmp(creator, GENERIC_CREATOR)!=0){
		printf("Messaggio mandato al server: %s\n", res);
		sendResult(res);
	}
}

void eraseDirectory(int inode){
	removeChild(inode);
	char* directoryname=(char*)calloc((strlen(array_fat[inode]->path)+strlen(array_fat[inode]->name)+2), sizeof(char));
	strcpy(directoryname, array_fat[inode]->path);
	strcat(directoryname, array_fat[inode]->name);
	int ret=remove(directoryname);
	printf("errno %d\n", errno);
	if(ret==-1){
		printf("La directory %s non esiste nella directory corrente\n", directoryname);
	}else{
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		char in[10]={0};
		sprintf(in, "%d\n", inode);
		insertInFatFile(in, inode);
	}
	free(directoryname);
}


void removeChild(int inode){
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			if(array_fat[i]->inode_padre==inode){
				if(strcmp(array_fat[i]->type, DIR_TYPE)==0){
					removeChild(i);
					eraseDirectory(i);
				}else{
					eraseFile(i);
				}
			}
		}
	}
}

void nextInode(){  //DA RIVEDERE
	int trovato=0;
	int i=0;
	char buffer[80]={0};
	int ret=0;
	FILE* fdfat=fopen(FAT_FILE_NAME, "r");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire il file in lettura");
	while(buffer[0]!=EOF && !trovato){
		fgets(buffer, 80, fdfat);
		char* str=strtok(buffer, SEPARATOR);
		str=strtok(NULL, SEPARATOR);
		if(!str){
			next_inode=i;
			trovato=1;
		}
		i++;
	}
	if(!trovato){
		next_inode=INODE_LIMIT;
	}
	printf("Inode calcolato: %d\n", next_inode);
	ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");
}

void insertInFatFile(char* row, int inode){
	FILE* fdfat=fopen(FAT_FILE_NAME, "r+");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	FILE* fd=fopen(AUX, "w+");
	if(fd==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il appoggio\n");
	char buffer[100]={0};
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, fdfat);
		if(i==inode){
			fputs(row, fd);		
		}else{
			fputs(buffer, fd);
		}
	}
	int ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");
	ret=fclose(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file di appoggio\n");
	
	fdfat=fopen(FAT_FILE_NAME, "w");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	fd=fopen(AUX, "r");
	if(fd==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file di appoggio\n");
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, fd);
		fputs(buffer, fdfat);
	}
	ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");
	ret=fclose(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file di appoggio\n");
	ret=remove(AUX);
	if(ret==-1) handle_error("Errore: impossibile rimuovere il file di appoggio\n");
}

void loadFAT(){
	FILE* FAT=fopen(FAT_FILE_NAME, "r");
	if(FAT==NULL) handle_error("Errore: impossibile aprire FAT.txt in lettura\n");
	char buffer[100]={0};
	char* token;
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, FAT);
		token=strtok(buffer, SEPARATOR);
		token=strtok(NULL, SEPARATOR);
		if(token!=NULL){			
			char* nome=token;
			char* path=strtok(NULL, SEPARATOR);
			char* tipo=strtok(NULL, SEPARATOR);
			char* creatore=strtok(NULL, SEPARATOR);
			int padre=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			array_fat[i]=(struct fat*)calloc(1, sizeof(struct fat));
			array_fat[i]->inode=i;
			strcpy(array_fat[i]->name, nome);
			strcpy(array_fat[i]->path, path);
			strcpy(array_fat[i]->type, tipo);
			strcpy(array_fat[i]->creator, creatore);
			array_fat[i]->inode_padre=padre;
		}else{
			array_fat[i]=NULL;
		}
	}
	int ret=fclose(FAT);
	if(ret==-1) handle_error("Errore: impossibile chiudere FAT.txt\n");
}

//CONDIVISIONE DELL'ARRAY DI ELEMENTI
void sharing_father(){
	int size=sizeof(struct fat)*MAX_INODE;
	int id=shmget(12345, size, IPC_CREAT | 0666);
	if(id==-1) handle_error("Error shmid\n");
	struct fat* shmem;
	shmem=(struct fat*)shmat(id, 0, 0);
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			shmem[i].inode=array_fat[i]->inode;
			strcpy(shmem[i].name, array_fat[i]->name);
			strcpy(shmem[i].path, array_fat[i]->path);
			strcpy(shmem[i].creator, array_fat[i]->creator);
			strcpy(shmem[i].type, array_fat[i]->type);
			shmem[i].inode_padre=array_fat[i]->inode_padre;
		}else{
			shmem[i].inode=INODE_LIMIT;
			strcpy(shmem[i].name, "");
			strcpy(shmem[i].path, "");
			strcpy(shmem[i].creator, "");
			strcpy(shmem[i].type, "");
			shmem[i].inode_padre=0;
		}
	}
	shmdt((void *)shmem);
}

void stampaArray(){
	printf("Stampa array:\n");
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			printf("Inode %d, creator %s, nome %s,\npercorso %s, tipo %s, inode padre %d\n\n", array_fat[i]->inode, array_fat[i]->creator, array_fat[i]->name, array_fat[i]->path, array_fat[i]->type, array_fat[i]->inode_padre);
		}
	}
}

int searchElement(char* name, char* path, char type){
	printf("Ricerca dell'elemento...");
	int inode;
	int trovato=0;
	for(int i=0; i<MAX_INODE && !trovato; i++){
		if(array_fat[i]!=NULL){
			if(strcmp(array_fat[i]->name, name)==0 && strcmp(array_fat[i]->path, path)==0 && array_fat[i]->type[0]==type){
				inode=i;
				trovato=1;
			}
			printf("%c %c\n", array_fat[i]->type[0], type);
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

void sendResult(char* res){
	printf("Preparazione invio risultato all'utente\n");
	int fdfifo=open(FIFO_FOR_RES, O_WRONLY);
	printf("Fifo aperta\n");
	if(fdfifo==-1){printf("errno: %d\n", errno); handle_error("Errore: impossibile aprire la fifo per comunicare con gli utenti\n");}
	int send_bytes=0;
	int size_elem=strlen(res);
	int ret=0;
	printf("Inviando '%s' all'utente...\n", res);
	while(send_bytes<size_elem){
		ret=write(fdfifo, res+send_bytes, size_elem-send_bytes);
		if(ret==-1){
			if(errno==EINTR){
				continue;
			}else{
				handle_error("Errore: impossibile mandare il messaggio all'utente\n");
			}
		}else{
			send_bytes+=ret;
		}
	}
	ret=close(fdfifo);
	if(ret==-1) handle_error("Errore: impossibile chiudere il canale di comunicazione\n");
}
