#include "fun_server.h"
#include "var.h"
#include "fun.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void nextInode(){  //DA RIVEDERE
	int trovato=0;
	int i=0;
	char buffer[80];
	int ret=0;
	FILE* fdfat=fopen(FAT_FILE_NAME, "r");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire il file in lettura");
	while(*buffer!=EOF && !trovato){
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
	char buffer[100];
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
	char buffer[100];
	char* token;
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, FAT);
		token=strtok(buffer, SEPARATOR);
		token=strtok(NULL, SEPARATOR);
		if(token!=NULL){			
			char* nome=token;
			printf("%s\n", nome);
			char* path=strtok(NULL, SEPARATOR);
			char* tipo=strtok(NULL, SEPARATOR);
			char* creatore=strtok(NULL, SEPARATOR);
			int size=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			int padre=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			array_fat[i]=(struct fat*)malloc(sizeof(struct fat));
			array_fat[i]->inode=i;
			//array_fat[i]->name=(char*)malloc(sizeof(char)*strlen(nome));
			strcpy(array_fat[i]->name, nome);
			//array_fat[i]->path=(char*)malloc(sizeof(char)*strlen(path));
			strcpy(array_fat[i]->path, path);
			//array_fat[i]->type=(char*)malloc(sizeof(char)*strlen(tipo)); 
			strcpy(array_fat[i]->type, tipo);
			//array_fat[i]->type=tipo;
			array_fat[i]->size=size;//DA RIVEDERE
			//array_fat[i]->creator=(char*)malloc(sizeof(char)*strlen(creatore));
			strcpy(array_fat[i]->creator, creatore);
			array_fat[i]->inode_padre=padre;
			printf("Inode %d, size %d, creator %s, nome %s, percorso %s, tipo %s\n", array_fat[i]->inode, array_fat[i]->size, array_fat[i]->creator, array_fat[i]->name, array_fat[i]->path, array_fat[i]->type);
		}else{
			array_fat[i]=NULL;
		}
	}
	int ret=fclose(FAT);
	if(ret==-1) handle_error("Errore: impossibile chiudere FAT.txt\n");
}

void sharing_father(){
	int size=sizeof(struct fat)+sizeof(char)*192;
	/*struct fat* shared=(struct fat*)malloc(sizeof(struct fat));
	shared->name=(char*)malloc(sizeof(char)*64);
	shared->path=(char*)malloc(sizeof(char)*64);
	shared->type=(char*)malloc(sizeof(char)*64);
	shared->creator=(char*)malloc(sizeof(char)*64);*/
	int fd=shm_open(SHMEM_FOR_INFO, O_CREAT|O_RDWR, 0666);
	if(fd==-1) handle_error("Errore: impossibile creare la zona di memoria condivisa\n");
	int ret=ftruncate(fd, size);
	if(ret==-1) handle_error("Errore: impossibile impostare una dimensione della zona di memoria condivisa\n");
	struct fat* shared=(struct fat*)mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0);
	if(shared==MAP_FAILED) handle_error("Errore: impossibile mappa la zona di memoria condivisa\n");
	printf("Elemento caricato\n");
	memcpy(shared, array_fat[1], size);
	strcpy(shared->name, array_fat[1]->name);
	strcpy(shared->path, array_fat[1]->path);
	strcpy(shared->type, array_fat[1]->type);
	strcpy(shared->creator, array_fat[1]->creator);
	printf("Inode padre %d\n", shared->inode);
	printf("Nome padre %s\n", shared->name);
	printf("Path padre %s\n", shared->path);
	printf("Type padre %s\n", shared->type);
	printf("Creator padre %s\n", shared->creator);
	ret=close(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file descriptor della memoria condivisa");	
}

void stampaArray(){
	printf("Stampa array:\n");
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			printf("Inode %d, size %d, creator %s, nome %s, percorso %s, tipo %s\n", array_fat[i]->inode, array_fat[i]->size, array_fat[i]->creator,
				 array_fat[i]->name, array_fat[i]->path, array_fat[i]->type);
		}else{
			//printf("Inode %d, NULL\n", i);
		}
	}
}

void searchElement(char* name, char* path, char* type){
	int trovato=0;
	int inode;
	for(int i=0; i<MAX_INODE && !trovato; i++){
		if(strcmp(array_fat[i]->name, name)==0 && strcmp(array_fat[i]->path, path)==0){
			inode=array_fat[i]->inode;
			trovato=1;
		}
	}
	if(trovato){
		if(strcmp(type, DIR_TYPE)==0){
			eraseDirectory(inode);
		}else{
			eraseFile(inode);
		}
	}
}
