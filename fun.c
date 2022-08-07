#include "fun.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//FUNZONI PER I FILE
void createFile(int fd, int inode, char* filename, char type, char* creator){
	int ret;
	if(next_inode<MAX_INODE){		
		int create=creat(pathname, O_RDWR);
		if(create==-1){
			if(errno==EEXIST){
				printf("Errore: impossibile creare il file chiamato '%s' perché esiste già un altro file o directory con quel nome\n", directoryname);
			}else{
				printf("Errore: impossbile creare il file\n");
			}
		}else{
			FAT* filefat=(FAT*)malloc(sizeof(FAT));
			filefat->inode=next_inode;
			filefat->size=0;
			filefat->creator=creator;
		
			file_struct* sfile=(file_struct*)malloc(sizeof(file_struct));
			sfile->fat=filefat;
			sfile->name=filename;
			//GESTIONE CREAZIONE AVVENUTA CON SUCCESSO
			next_inode++;
			//AGGIUNGERE INSERIMENTO NEL FILE FAT.txt
			char* elem;
			sprintf(elem, "%d %s %c %d\n", inode, filename, type, creator);
			int size_elem=strlen(elem);
			if(strcmp(creator, GENERIC_CREATOR)==0){
				addElement(fd, elem, size_elem);
			}else{
				int fdfifo=open(FIFO_FOR_FAT, O_WROLNY);
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
			}
		}
	}else{
		//GESTIONE IMPOSSIBLITA' DI CREARE IL FILE
		printf("Impossibile creare il nuovo file, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
	}
	
}

void eraseFile(char* filename){
	file_struct* sfile;
	//CHECK ESISTENZA DEL FILE (CERCARE E SALVARE LA STRUTTURA DEL FILE)
	if(/*ESISTENZA A BUON FINE*/){
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
void createDirectory(char* creator, char* directoryname){
	if(next_inode<MAX_INODE){
		int fddir=mkdir(pathname, creator);
		if(fddir==-1){
			if(errno==EEXIST){
				printf("Errore: impossibile creare la directory chiamata '%s' perché esiste già un altro file o directory con quel nome\n", directoryname);
			}else{
				printf("Errore: impossbile creare la directory\n");
			}
		}else{
			FAT* filefat=(FAT*)malloc(sizeof(FAT));
			filefat->inode=next_inode;
			filefat->size=0;
			filefat->creator=creator;
			
			file_struct* sfile=(file_struct*)malloc(sizeof(file_struct));
			sfile->fat=*filefat;
			sfile->name=filename;
			
			list_file* list=(list_file*)malloc(sizeof(list_file));
			list->file=NULL;
			list->next=NULL;
			
			directory_struct* sdirectory=(directory_struct*)malloc(sizeof(directory_struct));
			sdirectory->file=*sfile;
			sdirectory->list=list;
				
			printf("Directory create con successo\n");
			next_inode++;
			//AGGIUNGERE INSERIMENTO NEL FILE FAT.txt
			char* elem;
			sprintf(elem, "%d %s %c %d\n", inode, filename, type, creator);
			int size_elem=strlen(elem);
			if(strcmp(creator, GENERIC_CREATOR)==0){
				addElement(fd, elem, size_elem);
			}else{
				int fdfifo=open(FIFO_FOR_FAT, O_WROLNY);
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
			}
		}
	}else{
		//GESTIONE IMPOSSIBILITA' DI CREARE LA DIRECTORY
		printf("Impossibile creare la nuova directory, limite di file disponibili raggiunto\n");
		printf("Modifica o elimina file/directory già esistenti\n");
		
	}
}

void eraseDirectory(char* directoryname){
	directory_struct* sdirectory;
	//CHECK ESISTENZA DELLA DIRECTORY (CERCARE E SALVARE LA STRUTTURA DELLA DIRECTORY)
	if(/*ESISTENZA A BUON FINE*/){
		while(sdirectory->list!=NULL){
			//RICONOSCI TIPO ELEMENTO
			if(/*FILE*/){
				eraseFile(sdirectory->list->elem->name);
			}else{//DIRECTORY
				eraseDirectory(sdirectory->list->elem->name);
			}
		}
		free(sdyrectory->list);
		free(sdyrectory);
		next_inode--;
		//RIMOZIONE DAL FILE FAT.txt
		//GESTIONE ELIMINAZIONE DELLA DIRECTORY AVVENUTA CON SUCCESSO
	}else{
		//GESTIONE NON ESISTENZA DELLA DIRECTORY
	}
}



//FUNZIONI EXTRA
int rowCounter(int fd){
	char buffer;
	int count=0;
	
	while(buffer!=EOF){
		fread(buffer, sizeof(char), 1, fd);
		if(buffer=="\n"){
			count++;
		}
	}
	return count;
}

void addElement(int fd, char* elem, int size_elem){	
	int ret=write(fd, elem, size_elem);
	if(ret==-1){
		handle_error("Errore: impossibile aggiornare il file\n");
	}
	printf("File aggiornato correttamente\n");
}
