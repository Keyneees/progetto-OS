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
	/*FILE* fdfat=fopen(FAT_FILE_NAME, "r+");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	int write=0;
	char buffer[100];
	printf("Riga %s", row);
	char s[100];
	strcpy(s, row);
	char* inode=strtok(s, SEPARATOR);
	printf("Riga %s\n", row);
	while(!write){
		if(strcmp(inode, "0")==0){
			fprintf(fdfat, "%s", row);
			write=1;
			printf("Scritto buffer %s\n", row);
		}else{
			fgets(buffer, 10, fdfat);
			printf("riga letta: %s\n", buffer);
			char* str=strtok(buffer, SEPARATOR);
			printf("inode %s str %s%d\n", inode, str, strcmp(inode, str));
			if(strcmp(inode, str)==1){
				fprintf(fdfat, "%s", row);
				write=1;
				printf("Scritto buffer %s\n", row);
			}
		}
		exit(1);
	}
	int ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");*/
	FILE* fdfat=fopen(FAT_FILE_NAME, "r+");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	FILE* fd=fopen("appoggio.txt", "w+");
	if(fd==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
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
	fd=fopen("appoggio.txt", "r");
	if(fd==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, fd);
		fputs(buffer, fdfat);
	}
	ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");
	ret=fclose(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file di appoggio\n");
	ret=remove("appoggio.txt");
	if(ret==-1) handle_error("Errore: impossibile rimuovere il file di appoggio\n");
	
}
