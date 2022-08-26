#include "fun.h"
#include "var.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

//FUNZONI PER I FILE
void createFile(int inode, char* filename, char type, char* creator){
	int ret;
	if(next_inode<MAX_INODE){
		//char* pathname=*current_path+"/"+*filename;
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
			/*struct FAT* filefat=(struct FAT*)malloc(sizeof(struct FAT));
			filefat->inode=next_inode;
			filefat->size=0;
			filefat->creator=creator;
		
			struct file_struct* sfile=(struct file_struct*)malloc(sizeof(struct file_struct));
			sfile->fat=filefat;
			sfile->name=filename;
			sfile->path=current_path;
			array_fat[next_inode]=filefat;*/
			struct sfile* sfile=(struct sfile*)malloc(sizeof(struct sfile));
			sfile->nome=filename;
			sfile->path=current_path;
			sfile->type=type;
			
			struct fat* fat_elem=(struct fat*)malloc(sizeof(struct fat));
			fat_elem->inode=next_inode;
			fat_elem->size=0;
			fat_elem->creator=creator;
			fat_elem->sfile=(struct sdirectory*)sfile;
			printf("Creazione avvenuta con successo\n");
			//GESTIONE CREAZIONE AVVENUTA CON SUCCESSO
			//next_inode++; //INODE AGGIORNANTO DAL SERVER
			//AGGIUNGERE INSERIMENTO NEL FILE FAT.txt
			if(strcmp(creator, GENERIC_CREATOR)!=0){	
				fat_elem->inode_padre=fat_padre->inode;
				char elem[100];
				printf("Preparo la stringa\n");
				sprintf(elem, "%d %s %s %c %s %d %d\n", inode, filename, current_path, type, creator, 0, fat_elem->inode_padre);
				printf("Stringa: %s\n", elem);
				sendToServer(elem);
			}else{
				fat_elem->inode_padre=SERVER_FATHER;
			}
			array_fat[next_inode]=fat_elem;
		}
	}else{
		//GESTIONE IMPOSSIBLITA' DI CREARE IL FILE
		printf("Impossibile creare il nuovo file, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
	}
	
}

void eraseFile(int inode){
	char* filename=array_fat[inode]->sfile->sfile->nome;
	int ret=remove(filename);
	if(ret==-1){
		printf("Il file %s non esiste nella directory corrente\n", filename);
	}else{
		free(array_fat[inode]->sfile);
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		
		char elem[100];
		printf("Preparo la stringa\n");
		sprintf(elem, "%s %d\n", DELETE_CMD, inode);
		printf("Stringa: %s\n", elem);
		sendToServer(elem);
	}
	
}

//FUNZIONI PER LE DIRECTORY
void createDirectory(int inode, char* directoryname, char type, char* creator){
	if(next_inode<MAX_INODE){
		//char* pathname=*current_path+"/"+*directoryname;
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
			/*struct FAT* filefat=(struct FAT*)malloc(sizeof(struct FAT));
			filefat->inode=next_inode;
			filefat->size=0;
			filefat->creator=creator;
			
			struct file_struct* sfile=(struct file_struct*)malloc(sizeof(struct file_struct));
			sfile->fat=filefat;
			sfile->name=directoryname;
			sfile->path=current_path;
			
			struct list_file* list=(struct list_file*)malloc(sizeof(struct list_file));
			list->elem=NULL;
			list->next=NULL;
			
			struct directory_struct* sdirectory=(struct directory_struct*)malloc(sizeof(struct directory_struct));
			sdirectory->file=*sfile;
			sdirectory->list=list;
			
			printf("Directory create con successo\n");
			array_fat[next_inode]=filefat;*/
			printf("Start\n");
			struct sdirectory* sdirectory=(struct sdirectory*)malloc(sizeof(struct sdirectory));
			printf("bingo\n");
			sdirectory->sfile=(struct sfile*)malloc(sizeof(struct sfile));
			sdirectory->sfile->nome=directoryname;
			printf("as\n");
			sdirectory->sfile->path=current_path;
			printf("sa\n");
			sdirectory->sfile->type=type;
			printf("here...");
			sdirectory->element=(int*)calloc(MAX_INODE, sizeof(int));
			printf("no\n");
			struct fat* fat_elem=(struct fat*)malloc(sizeof(struct fat));
			fat_elem->inode=next_inode;
			fat_elem->size=0;
			fat_elem->creator=creator;
			printf("here... ");
			fat_elem->sfile=sdirectory;
			printf("no\n");
			//next_inode++;//INODE AGGIORNATO DAL SERVER
			//AGGIUNGERE INSERIMENTO NEL FILE FAT.txt
			if(strcmp(creator, GENERIC_CREATOR)!=0){
				fat_elem->inode_padre=fat_padre->inode;
				char elem[100];
				printf("Preparo la stringa\n");
				sprintf(elem, "%d %s %s %c %s %d %d\n", inode, directoryname, current_path, type, creator, 0, fat_elem->inode_padre);
				printf("Stringa: %s\n", elem);
				sendToServer(elem);
			}else{
				fat_elem->inode_padre=SERVER_FATHER;
			}
			array_fat[next_inode]=fat_elem;
			
		}
	}else{
		//GESTIONE IMPOSSIBILITA' DI CREARE LA DIRECTORY
		printf("Impossibile creare la nuova directory, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
		
	}
}

void eraseDirectory(int inode){
	char* directoryname=array_fat[inode]->sfile->sfile->nome;
	int ret=rmdir(directoryname);
	if(ret==-1){
		printf("La directory %s non esiste nella directory corrente", directoryname);
	}else{
		for(int i=0; i<MAX_INODE; i++){
			if(array_fat[inode]->sfile->element[i]==1){
				char type=array_fat[i]->sfile->sfile->type;
				if(type==DIR_TYPE){
					eraseDirectory(i);
				}else{
					eraseFile(i);
				}
			}
		}
		free(array_fat[inode]->sfile->element);
		free(array_fat[inode]->sfile);
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		
		char elem[100];
		printf("Preparo la stringa\n");
		sprintf(elem, "%s %d\n", DELETE_CMD, inode);
		printf("Stringa: %s\n", elem);
		sendToServer(elem);
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

void sendToServer(char* elem){
	int ret;
	int size_elem=strlen(elem);
	printf("Mi preparo per aggiungere elementi nella fat\n");
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
}

void sizeUpdate(int inode){
	if(inode>=0 && inode<MAX_INODE){
		struct stat s;
		char* filename=array_fat[inode]->sfile->sfile->nome;
		printf("Pronto ad aggiornare la size del file %s\n", filename);
		int ret=stat(filename, &s);
		printf("Fatto...");
		printf("Dimensione dell'elemento di inode %d: %ld\n", inode, s.st_size);
		array_fat[inode]->size=s.st_size;
		
		if(strcmp(array_fat[inode]->creator,GENERIC_CREATOR)){
			char elem[100];
			sprintf(elem, "%s %d %s %s %c %s %d %d\n", UPDATE_CMD, inode, array_fat[inode]->sfile->sfile->nome, 
			array_fat[inode]->sfile->sfile->path, array_fat[inode]->sfile->sfile->type, array_fat[inode]->creator, array_fat[inode]->size, 
			array_fat[inode]->inode_padre);
			sendToServer(elem);
		}
		sizeUpdate(array_fat[inode]->inode_padre);
	}
}
