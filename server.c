#include "fun.h"
#include "var.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int fdfat;
char* buf;

int main(){
	struct stat* sfat;
	int exist=stat(FAT_FILE_NAME, sfat); //VERIFICA L'ESISTENZA DEL FAT.txt
	if(exist){
		createFile(fdfat, next_inode, FAT_FILE_NAME, FILE_TYPE, GENERIC_CREATOR);
		createDirectory(fdfat, next_inode, FILE_SYSTEM_DIRECTORY, DIR_TYPE, GENERIC_CREATOR);
		fdfat=open(FAT_FILE_NAME, O_RDWR);
		if(fdfat==-1) handle_error("Errore: impossibile aprire il FAT.txt\n");
		for(int i=0; i<MAX_INODE; i++){
			char buffer[10];
			sprintf(buffer, "%d\n", i);
			write(fdfat, buffer, strlen(buffer));
		}
		nextInode(fdfat);
		//INSERIRE FILE E DIRECTORY SUL FAT.txt
	}else{
		
	}
	int ret;
	
	printf("Server pronto all'avvio\n");
	
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
		printf("%s\n", elem);
		if(strcmp(elem, DELETE_CMD)==0){
			//GESTIONE ELIMINAZIONE DELLA RIGA
		}else{
			//GESTIONE INSERIMENTO DELLA RIGA
		} 
	}
}
