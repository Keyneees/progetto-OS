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
void createFile(int fd, int inode, char* filename, char type, char* creator){
	int ret;
	if(next_inode<MAX_INODE){
		char* pathname=filename;//DA DEFINIRE
		int create=creat(pathname, PERMESSI_FILE);
		if(create==-1){
			if(errno==EEXIST){
				printf("Errore: impossibile creare il file chiamato '%s' perché esiste già un altro file o directory con quel nome\n", filename);
			}else{
				printf("Errore: impossbile creare il file\n");
			}
		}else{
			printf("File %s creato con successo\n", filename);
			printf("Creazione della struttura\n");
			struct FAT* filefat=(struct FAT*)malloc(sizeof(struct FAT));
			filefat->inode=next_inode;
			filefat->size=0;
			filefat->creator=creator;
		
			struct file_struct* sfile=(struct file_struct*)malloc(sizeof(struct file_struct));
			sfile->fat=filefat;
			sfile->name=filename;
			array_fat[next_inode]=filefat;
			printf("Creazione avvenuta con successo\n");
			//GESTIONE CREAZIONE AVVENUTA CON SUCCESSO
			//next_inode++; //INODE AGGIORNANTO DAL SERVER
			//AGGIUNGERE INSERIMENTO NEL FILE FAT.txt
			if(strcmp(creator, GENERIC_CREATOR)!=0){
				sendToServer(inode, filename, type, creator);
			}
		}
	}else{
		//GESTIONE IMPOSSIBLITA' DI CREARE IL FILE
		printf("Impossibile creare il nuovo file, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
	}
	
}

void eraseFile(char* filename){
	struct file_struct* sfile;
	//CHECK ESISTENZA DEL FILE (CERCARE E SALVARE LA STRUTTURA DEL FILE)
	if(/*ESISTENZA A BUON FINE*/1){
		free(sfile->fat);
		free(sfile);
		next_inode--;
		//RIMOZIONE DAL FILE FAT.txt
		//GESTIONE ELIMINAZIONE DEL FILE AVVENUTA CON SUCCESSO
	}else{
		//GESTIONE NON ESISTENZA DEL FILE
	}
}

//FUNZIONI PER LE DIRECTORY
void createDirectory(int fd, int inode, char* directoryname, char type, char* creator){
	if(next_inode<MAX_INODE){
		char* pathname=directoryname;
		int fddir=mkdir(pathname, PERMESSI_DIRECTORY);
		if(fddir==-1){
			if(errno==EEXIST){
				printf("Errore: impossibile creare la directory chiamata '%s' perché esiste già un altro file o directory con quel nome\n", directoryname);
			}else{
				printf("Errore: impossbile creare la directory\n");
			}
		}else{
			struct FAT* filefat=(struct FAT*)malloc(sizeof(struct FAT));
			filefat->inode=next_inode;
			filefat->size=0;
			filefat->creator=creator;
			
			struct file_struct* sfile=(struct file_struct*)malloc(sizeof(struct file_struct));
			sfile->fat=filefat;
			sfile->name=directoryname;
			
			struct list_file* list=(struct list_file*)malloc(sizeof(struct list_file));
			list->elem=NULL;
			list->next=NULL;
			
			struct directory_struct* sdirectory=(struct directory_struct*)malloc(sizeof(struct directory_struct));
			sdirectory->file=*sfile;
			sdirectory->list=list;
			
			printf("Directory create con successo\n");
			array_fat[next_inode]=filefat;
			//next_inode++;//INODE AGGIORNATO DAL SERVER
			//AGGIUNGERE INSERIMENTO NEL FILE FAT.txt
			if(strcmp(creator, GENERIC_CREATOR)!=0){
				sendToServer(inode, directoryname, type, creator);
			}
			
		}
	}else{
		//GESTIONE IMPOSSIBILITA' DI CREARE LA DIRECTORY
		printf("Impossibile creare la nuova directory, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
		
	}
}

void eraseDirectory(char* directoryname){
	struct directory_struct* sdirectory;
	//CHECK ESISTENZA DELLA DIRECTORY (CERCARE E SALVARE LA STRUTTURA DELLA DIRECTORY)
	if(/*ESISTENZA A BUON FINE*/1){
		while(sdirectory->list!=NULL){
			//RICONOSCI TIPO ELEMENTO
			if(/*FILE*/1){
				eraseFile(sdirectory->list->elem->name);
			}else{//DIRECTORY
				eraseDirectory(sdirectory->list->elem->name);
			}
		}
		free(sdirectory->list);
		free(sdirectory);
		next_inode--;
		//RIMOZIONE DAL FILE FAT.txt
		//GESTIONE ELIMINAZIONE DELLA DIRECTORY AVVENUTA CON SUCCESSO
	}else{
		//GESTIONE NON ESISTENZA DELLA DIRECTORY
	}
}



//FUNZIONI EXTRA

void addElement(int fd, char* elem, int size_elem){
	printf("Sono in addElement\n");
	int ret=write(fd, elem, size_elem);
	if(ret==-1){
		handle_error("Errore: impossibile aggiornare il file\n");
	}
	printf("File aggiornato correttamente\n");
}

void sendToServer(int inode, char* name, char type, char* creator){
	int ret;
	char elem[100];
	printf("Preparo la stringa\n");
	sprintf(elem, "%d %s %c %s\n", inode, name, type, creator);
	printf("Stringa: %s\n", elem);
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
