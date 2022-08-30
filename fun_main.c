#include "fun_main.h"
#include "fun.h"
#include "var.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void printInfo(){
	printf("In questo sistema potrai eseguire diverse operazioni su file e directory con i seguenti comandi:\n");
	printf("\tcf -> comando per creare un nuovo file\n");
	printf("\tef -> comando per eliminare un file\n");
	printf("\twf -> comando per scrivere in un file\n");
	printf("\trf -> comando per leggere il contenuto di un file\n");
	printf("\tsf -> comando per spostare il cursore dentro al file\n");
	printf("\tcd -> comando per creare una nuova directory\n");
	printf("\ted -> comando per eliminare una directory\n");
	printf("\tmd -> comando per cambiare directory corrente\n");
	printf("\tld -> comando per stampare gli elementi presenti nella direcoty corrente\n");
	printf("\thelp -> comando per visualizzare i comandi eseguibili nel sistema\n");
	printf("\texit -> comando per chiudere il sistema\n");
}

void sharing_father(){
	int fd=shm_open(SHMEM_FOR_INFO, O_RDONLY, 0666);
	if(fd==-1) handle_error("Errore: impossibile aprire la memoria condivisa\n");
	int size=sizeof(struct fat*);
	fat_padre=(struct fat*)mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
	if(fat_padre==MAP_FAILED) handle_error("Errore: impossibile recuperare dati dal memoria condivisa\n");
	printf("Padre ricevuto %p", fat_padre);
	int ret=close(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere la memoria condivisa\n");
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

