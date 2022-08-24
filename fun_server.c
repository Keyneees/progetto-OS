#include "fun_server.h"
#include "var.h"
#include "fun.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void nextInode(int fdfile){  //DA RIVEDERE
	int trovato=0;
	int i=0;
	char buffer[80];
	int ret=0;
	FILE* fdfat=fopen(FAT_FILE_NAME, "r");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire il file in lettura");
	while(buffer && !trovato){
		fgets(buffer, 80, fdfat);
		printf("buffer: %s\n", buffer);
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

void deleteInFatFile(int inode){
	FILE* fdfat=fopen(FAT_FILE_NAME, "r+");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	FILE* fd=fopen(AUX, "w+");
	if(fd==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file di appoggio\n");
	char buffer[100];
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, fdfat);
		if(i==inode){
			char row[100];
			sprintf(row, "%d\n", inode);
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
	char* buffer;
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
			int size=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			int padre=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			struct fat* fat_elem=(struct fat*)malloc(sizeof(struct fat));
			fat_elem->inode=i;
			fat_elem->size=size;
			fat_elem->creator=creatore;
			fat_elem->inode_padre=padre;
			if(*tipo==DIR_TYPE){
				struct sdirectory* sdir=(struct sdirectory*)malloc(sizeof(struct sdirectory));
				sdir->sfile=(struct sfile*)malloc(sizeof(struct sfile));
				sdir->sfile->nome=nome;
				sdir->sfile->path=path;
				sdir->sfile->type=DIR_TYPE;
				sdir->element=calloc(MAX_INODE, sizeof(int*));
				fat_elem->sfile=sdir;
			}else{
				struct sfile* sfile=(struct sfile*)malloc(sizeof(struct sfile));
				sfile->nome=nome;
				sfile->path=path;
				sfile->type=FILE_TYPE;
				fat_elem->sfile=(struct sdirectory*)sfile;
			}
			array_fat[i]=fat_elem;
		}else{
			array_fat[i]=NULL;
		}
	}
	int ret=fclose(FAT);
	if(ret==-1) handle_error("Errore: impossibile chiudere FAT.txt\n");
	
	int inode;
	int inode_padre;
	for(int i=0; i<MAX_INODE; i++){
		inode=array_fat[i]->inode;
		inode_padre=array_fat[i]->inode_padre;
		if(inode_padre>=0 && inode_padre<MAX_INODE){
			array_fat[inode_padre]->sfile->element[inode]=1;
		}
	}
}

void updateFat(int inode, int size){
	
}
