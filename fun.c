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
void createFile(int inode, char* filename, char* type, char* creator, char* path, int inode_padre){
	int ret;
	char res[128];
	memset(res, 0, 128);
	if(next_inode<MAX_INODE){
		char* location;
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
			array_fat[next_inode]=(struct fat*)malloc(sizeof(struct fat));
			array_fat[next_inode]->inode=next_inode;
			//array_fat[next_inode]->name=(char*)malloc(sizeof(char)*strlen(filename));
			strcpy(array_fat[next_inode]->name, filename);
			//array_fat[next_inode]->path=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->path, path);
			//array_fat[next_inode]->type=(char*)malloc(sizeof(char));
			strcpy(array_fat[next_inode]->type, type);
			//array_fat[next_inode]->type=type;
			array_fat[next_inode]->size=0;
			//array_fat[next_inode]->creator=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->creator, creator);
			array_fat[next_inode]->inode_padre=inode_padre;
			sprintf(res, "Elemento creato con successo");
			printf("%s", res);
			char row[100];
			sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, filename, path, type, creator, 0, inode_padre);
			insertInFatFile(row, next_inode);
			printf("Fat file aggiornato\n");
		}
	}else{
		//GESTIONE IMPOSSIBLITA' DI CREARE IL FILE
		sprintf(res, "Impossibile creare il nuovo file, limite di file disponibili raggiunto\nModifica o elimina file/directory già esistenti\n");
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
	if(ret==-1){
		printf("Il file %s non esiste nella directory corrente\n", filename);
	}else{
		/*free(array_fat[inode]->name);
		free(array_fat[inode]->path);
		free(array_fat[inode]->type);
		free(array_fat[inode]->creator);*/
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
void createDirectory(int inode, char* directoryname, char* type, char* creator, char* path, int inode_padre){
	char res[128];
	if(next_inode<MAX_INODE){
		char* location;
		if(strcmp(path, "/")==0){
			location=(char*)malloc(sizeof(char)*(strlen(directoryname)));
			strcpy(location, directoryname);
		}else{
			location=(char*)malloc(sizeof(char)*(strlen(path)+strlen(directoryname)+2));
			strcpy(location, path);
			strcat(location, directoryname);
		}
		printf("Elementi dati -> path %s, nome %s", path, directoryname);
		printf("Location calcolata %s\n", location);
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
			array_fat[next_inode]=(struct fat*)malloc(sizeof(struct fat));
			array_fat[next_inode]->inode=inode;
			//array_fat[next_inode]->name=(char*)malloc(sizeof(char)*strlen(directoryname));
			strcpy(array_fat[next_inode]->name, directoryname);
			//array_fat[next_inode]->path=(char*)malloc(sizeof(char)*strlen(current_path));
			strcpy(array_fat[next_inode]->path, path);
			//array_fat[next_inode]->type=(char*)malloc(sizeof(char));
			strcpy(array_fat[next_inode]->type, type);
			//array_fat[next_inode]->type=type;
			array_fat[next_inode]->size=0;
			//array_fat[next_inode]->creator=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->creator, creator);
			array_fat[next_inode]->inode_padre=inode_padre;
			sprintf(res, "Elemento creato con successo");
			printf("Elemento inserito\n");
			char row[100];
			sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, directoryname, path, type, creator, 0, inode_padre);
			insertInFatFile(row, next_inode);
		}
	}else{
		//GESTIONE IMPOSSIBILITA' DI CREARE LA DIRECTORY
		sprintf(res, "Impossibile creare la nuova directory, limite di file disponibili raggiunto\nModifica o elimina file/directory già esistenti\n");
		printf("%s", res);
		
	}
	if(strcmp(creator, GENERIC_CREATOR)!=0){
		//sleep(3);
		printf("Messaggio mandato al server: %s\n", res);
		sendResult(res);
	}
}

void eraseDirectory(int inode){
	char* directoryname=(char*)malloc(sizeof(char)*(strlen(array_fat[inode]->path)+strlen(array_fat[inode]->name)+2));
	strcpy(directoryname, array_fat[inode]->path);
	strcat(directoryname, array_fat[inode]->name);
	int ret=remove(directoryname);
	if(ret==-1){
		printf("La directory %s non esiste nella directory corrente", directoryname);
	}else{
		removeChild(inode);
		/*free(array_fat[inode]->name);
		free(array_fat[inode]->path);
		free(array_fat[inode]->type);
		free(array_fat[inode]->creator);*/
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
			char elem[sizeof(struct fat)+64];
			sprintf(elem, "%d %s %s %s %s %d %d\n", inode, array_fat[inode]->name, 
			array_fat[inode]->path, array_fat[inode]->type, array_fat[inode]->creator, array_fat[inode]->size, 
			array_fat[inode]->inode_padre);
			insertInFatFile(elem, inode);
		}
		sizeUpdate(array_fat[inode]->inode_padre);
	}
}

void removeChild(int inode){
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			if(array_fat[i]->inode_padre==inode){
				if(strcmp(array_fat[i]->type, DIR_TYPE)==0){
					removeChild(i);
				}
				free(array_fat[inode]);
				array_fat[inode]=NULL;
			}
		}
	}
}
