#include "fun.h"
#include "var.h"
#include "fun_server.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int inode_fat;
int fdfat;
char* buf;

int main(){
	/*struct stat* sfat;
	int exist=stat(FAT_FILE_NAME, sfat); //VERIFICA L'ESISTENZA DEL FAT.txt*/
	current_path="/";
	FILE* exist=fopen(FAT_FILE_NAME, "r");
	if(exist==NULL){
		printf("Il file non esiste\n");
		createFile(next_inode, FAT_FILE_NAME, FILE_TYPE, GENERIC_CREATOR);
		createDirectory(next_inode, FILE_SYSTEM_DIRECTORY, DIR_TYPE, GENERIC_CREATOR);
		fdfat=open(FAT_FILE_NAME, O_RDWR);
		if(fdfat==-1) handle_error("Errore: impossibile aprire il FAT.txt\n");
		for(int i=0; i<MAX_INODE; i++){
			char buffer[10];
			sprintf(buffer, "%d\n", i);
			write(fdfat, buffer, strlen(buffer));
		}
		nextInode(fdfat);
		printf("Inode calcolato: %d\n", next_inode);
		inode_fat=next_inode;
		//INSERIRE FILE E DIRECTORY SUL FAT.txt
		char row[100];
		sprintf(row, "%d %s %s %c %s %d %d\n", next_inode, FAT_FILE_NAME, current_path, FILE_TYPE, GENERIC_CREATOR, 0, SERVER_FATHER);
		printf("row calcolata %s", row);
		insertInFatFile(row, next_inode);
		
		nextInode(fdfat);
		printf("Inode calcolato: %d\n", next_inode);
		//INSERIRE FILE E DIRECTORY SUL FAT.txt
		sprintf(row, "%d %s %s %c %s %d %d\n", next_inode, FILE_SYSTEM_DIRECTORY, current_path, DIR_TYPE, GENERIC_CREATOR, 0, SERVER_FATHER);
		insertInFatFile(row, next_inode);
		
		sizeUpdate(inode_fat);
	}else{
		printf("Il file esiste\n");
		//CARICARE LA STRUTTURA CHE TIENE MEMORIA DEI FILE E DELLE DIRECTORY CREATE
		loadFAT();
	}
	nextInode(fdfat);
	printf("Inode calcolato: %d\n", next_inode);
	int ret;
	
	printf("Server pronto all'avvio\n");
	current_path=*FILE_SYSTEM_DIRECTORY+"/";
	
	while(1){
		printf("In attesa dei comandi degli utenti...\n");
		ret=mkfifo(FIFO_FOR_FAT, 0600);
		if(ret==-1) handle_error("Errore: impossibile creare la fifo per ricevere istruzioni dagli utenti\n");
		
		int fdfifo=open(FIFO_FOR_FAT, O_RDONLY);
		if(fdfifo==-1) handle_error("Errore: impossibile aprire la fifo per comunicare con gli utenti\n");
		
		int bytes_read=0;
		while(1){
			bytes_read=read(fdfifo, buf+bytes_read, 1);
			if(bytes_read==-1){
				if(errno==EINTR){
					continue;
				}else{
					handle_error("Errore: impossibile leggere il messaggio dell'utente\n");
				}
			}else if(bytes_read==0){
				break;
			}else{
				bytes_read++;
			}
		}
		
		ret=close(fdfifo);
		if(ret==-1) handle_error("Errore: impossibile chiudere il descrittore della fifo\n");
		ret=unlink(FIFO_FOR_FAT);
		if(ret==-1) handle_error("Errore: impossibile eliminare la fifo\n");
		
		char* elem=strtok(buf, SEPARATOR);
		printf("Primo elemento ricevuto %s\n", elem);
		printf("%s\n", elem);
		if(strcmp(elem, DELETE_CMD)==0){
			//GESTIONE ELIMINAZIONE DELLA RIGA
			int inode=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			deleteInFatFile(inode);
			sizeUpdate(inode_fat);
			nextInode(fdfat);
		}else{
			//GESTIONE INSERIMENTO DELLA RIGA
			int inode=strtol(elem, NULL, 10);
			insertInFatFile(elem, inode);
			sizeUpdate(inode_fat);
			nextInode(fdfat);
		} 
	}
}
