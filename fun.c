#include "fun.h"
#include "var.h"
#include "fun_server.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

//FUNZONI PER I FILE
void createFile(int inode, char* filename, char* type, char* creator){
	int ret;
	if(next_inode<MAX_INODE){
		int create=creat(filename, PERMESSI_FILE);
		if(create==-1){
			if(errno==EEXIST){
				printf("Errore: impossibile creare il file chiamato '%s' perché esiste già un altro file o directory con quel nome\n", filename);
			}else{
				printf("Errore: impossbile creare il file\n");
			}
		}else{
			printf("File %s creato con successo\n", filename);
			printf("Creazione della struttura\n");
			array_fat[next_inode]=(struct fat*)malloc(sizeof(struct fat));
			array_fat[next_inode]->inode=next_inode;
			array_fat[next_inode]->name=(char*)malloc(sizeof(char)*strlen(filename));
			strcpy(array_fat[next_inode]->name, filename);
			array_fat[next_inode]->path=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->path, current_path);
			//array_fat[next_inode]->type=(char*)malloc(sizeof(char));
			//strcpy(array_fat[next_inode]->type, type);
			array_fat[next_inode]->type=type;
			array_fat[next_inode]->size=0;
			printf("Inserimento del creatore...");
			array_fat[next_inode]->creator=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->creator, creator);
			printf("Riuscita\n");
			if(strcmp(GENERIC_CREATOR, creator)==0){
				array_fat[next_inode]->inode_padre=SERVER_FATHER;
			}else{
				array_fat[next_inode]->inode_padre=99999;
			}
			printf("Elemento inserito\n");
		}
	}else{
		//GESTIONE IMPOSSIBLITA' DI CREARE IL FILE
		printf("Impossibile creare il nuovo file, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
	}
	
}

void eraseFile(int inode){
	char* filename=array_fat[inode]->name;
	int ret=remove(filename);
	if(ret==-1){
		printf("Il file %s non esiste nella directory corrente\n", filename);
	}else{
		free(array_fat[inode]->name);
		free(array_fat[inode]->path);
		free(array_fat[inode]->type);
		free(array_fat[inode]->creator);
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		
		/*char elem[100];
		printf("Preparo la stringa\n");
		sprintf(elem, "%s %d\n", DELETE_CMD, inode);
		printf("Stringa: %s\n", elem);
		sendToServer(elem);*/
	}
	
}

//FUNZIONI PER LE DIRECTORY
void createDirectory(int inode, char* directoryname, char* type, char* creator){
	if(next_inode<MAX_INODE){
		printf("lol");
		int fddir=mkdir(directoryname, PERMESSI_DIRECTORY);
		printf("Qui\n");
		if(fddir==-1){
			if(errno==EEXIST){
				printf("Errore: impossibile creare la directory chiamata '%s' perché esiste già un altro file o directory con quel nome\n", directoryname);
			}else{
				printf("Errore: impossbile creare la directory\n");
			}
		}else{
			array_fat[next_inode]=(struct fat*)malloc(sizeof(struct fat));
			array_fat[next_inode]->inode=inode;
			array_fat[next_inode]->name=(char*)malloc(sizeof(char)*strlen(directoryname));
			strcpy(array_fat[next_inode]->name, directoryname);
			array_fat[next_inode]->path=(char*)malloc(sizeof(char)*strlen(current_path));
			strcpy(array_fat[next_inode]->path, current_path);
			//array_fat[next_inode]->type=(char*)malloc(sizeof(char));
			/*strcpy(array_fat[next_inode]->type, type);*/
			array_fat[next_inode]->type=type;
			array_fat[next_inode]->size=0;
			array_fat[next_inode]->creator=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->creator, creator);
			if(strcmp(GENERIC_CREATOR, creator)==0){
				array_fat[next_inode]->inode_padre=SERVER_FATHER;
			}else{
				array_fat[next_inode]->inode_padre=99999;
			}
		}
	}else{
		//GESTIONE IMPOSSIBILITA' DI CREARE LA DIRECTORY
		printf("Impossibile creare la nuova directory, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
		
	}
}

void eraseDirectory(int inode){
	char* directoryname=array_fat[inode]->name;
	int ret=rmdir(directoryname);
	if(ret==-1){
		printf("La directory %s non esiste nella directory corrente", directoryname);
	}else{
		for(int i=0; i<MAX_INODE; i++){
			if(array_fat[i]->inode_padre==inode){
				if(strcmp(array_fat[i]->type, DIR_TYPE)==0){
					eraseDirectory(i);
				}else{
					eraseFile(i);
				}
			}
		}
		free(array_fat[inode]->name);
		free(array_fat[inode]->path);
		free(array_fat[inode]->type);
		free(array_fat[inode]->creator);
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		
		/*char elem[100];
		printf("Preparo la stringa\n");
		sprintf(elem, "%s %d\n", DELETE_CMD, inode);
		printf("Stringa: %s\n", elem);
		sendToServer(elem);*/
	}
}



//FUNZIONI EXTRA

void addElement(int fd, char* elem, int size_elem){ //DA RIVEDERE
	printf("Sono in addElement\n");
	int ret=write(fd, elem, size_elem);
	if(ret==-1){
		handle_error("Errore: impossibile aggiornare il file\n");
	}
	printf("File aggiornato correttamente\n");
}

void sizeUpdate(int inode){
	if(inode>=0 && inode<MAX_INODE){
		struct stat s;
		char* filename=array_fat[inode]->name;
		printf("Pronto ad aggiornare la size del file %s\n", filename);
		int ret=stat(filename, &s);
		printf("Fatto...");
		printf("Dimensione dell'elemento di inode %d: %ld\n", inode, s.st_size);
		array_fat[inode]->size=s.st_size;
		
		if(strcmp(array_fat[inode]->creator,GENERIC_CREATOR)){
			/*char elem[100];
			sprintf(elem, "%s %d %s %s %s %s %d %d\n", UPDATE_CMD, inode, array_fat[inode]->name, 
			array_fat[inode]->path, array_fat[inode]->type, array_fat[inode]->creator, array_fat[inode]->size, 
			array_fat[inode]->inode_padre);
			sendToServer(elem);*/
		}else{
			char elem[100];
			sprintf(elem, "%d %s %s %s %s %d %d\n", inode, array_fat[inode]->name, 
			array_fat[inode]->path, array_fat[inode]->type, array_fat[inode]->creator, array_fat[inode]->size, 
			array_fat[inode]->inode_padre);
			insertInFatFile(elem, inode);
		}
		sizeUpdate(array_fat[inode]->inode_padre);
	}
}
