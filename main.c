#include "fun_main.h"
#include "var.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//char* current_path;

int main(){
	array_fat=(struct fat**)malloc(sizeof(struct fat*)*MAX_INODE);
	
	//sem_unlink(SEM_SERVER);
	//server=malloc(sizeof(sem_t));
	server=NULL;
	server=sem_open(SEM_SERVER, O_CREAT);
	if(server==SEM_FAILED) handle_error("Errore: impossibile avviare sem_server\n");
	
	//sem_unlink(SEM_MAIN);
	main_s=NULL;
	main_s=sem_open(SEM_MAIN, O_CREAT);
	if(main_s==SEM_FAILED) handle_error("Errore: impossibile avviare sem_main\n");
	
	//sem_unlink(SEM_SHMEM);
	shmem=NULL;
	shmem=sem_open(SEM_SHMEM, O_CREAT);
	if(shmem==SEM_FAILED) handle_error("Errore: impossibile avviare sem_shmem\n");
	
	unlink(FIFO_FOR_RES);
	int ret=mkfifo(FIFO_FOR_RES, 0666);
	if(ret==-1) handle_error("Errore: impossibile creare la fifo per ricevere istruzioni dagli utenti\n");
	
	for(int i=0; i<MAX_INODE; i++){
		//array_fat[i]=(struct fat*)malloc(sizeof(struct fat));	
		array_fat[i]=NULL;
	}
	printf("%sInserire il nome per il login: ", CMD_LINE);
	fgets(user, 30, stdin);
	for(int i=0; i<30; i++){
		if(user[i]==10){
			user[i]='\0';
		}
	}
	while(strcmp(GENERIC_CREATOR, user)==0){
		printf("Errore: non è possibile usare il nome %s. Si prega di inserire un nome diverso\n", user);
		printf("%sInserire il nome per il login: ", CMD_LINE);
		//scanf("%s", user);
		fgets(user, 30, stdin);
		for(int i=0; i<30; i++){
			if(user[i]==10){
				user[i]='\0';
			}
		}
	}
	printf("Benvenuto '%s' nel sistema FAT_OS\n", user);
	/**/
	printInfo();
	printInfoMD();
	
	char row[50]={0};
	char info[50]={0};
	char name[50]={0};
	char elem[100]={0};
	//char* cmd=(char*)calloc(64, sizeof(char));
	char cmd[64]={0};
	int e=0;
	sharing_father();
	//stampaArray();
	/*inode_padre=fat_padre->inode;
	printf("Info padre:\n");
	printf("Puntantore: %p\n", fat_padre);
	printf("Puntatore nome %p\n", fat_padre->name);
	printf("inode %d\n", fat_padre->inode);
	printf("nome %s\n", fat_padre->name);
	printf("path %s\n", fat_padre->path);*/
	int trovato=0;
	int i=0;
	while(!trovato && i<MAX_INODE){
		if(memcmp(array_fat[i]->name, FILE_SYSTEM_DIRECTORY, strlen(FILE_SYSTEM_DIRECTORY))==0){
			fat_padre=(struct fat*)calloc(1, sizeof(struct fat));
			fat_padre->inode=array_fat[i]->inode;
			//fat_padre->size=array_fat[i]->size;
			fat_padre->inode_padre=array_fat[i]->inode_padre;
			memset(fat_padre->name, 0, 64);
			//strcpy(fat_padre->name, array_fat[i]->name);
			//int l=strlen(array_fat[i]->name);
			/*for(int k=0; k<64; k++){
				printf("name[%d] %c\n", k, array_fat[i]->name[k]);
			}*/
			copyArrayString(fat_padre->name, array_fat[i]->name);/*
			for(int j=0; j<l; j++){
				fat_padre->name[j]=array_fat[i]->name[j];
			}*/
			//strcpy(fat_padre->path, array_fat[i]->path);
			/*l=strlen(array_fat[i]->path);
			for(int j=0; j<l; j++){
				fat_padre->path[j]=array_fat[i]->path[j];
			}*/
			copyArrayString(fat_padre->path, array_fat[i]->path);
			//strcpy(fat_padre->type, array_fat[i]->type);
			/*l=strlen(array_fat[i]->type);
			for(int j=0; j<l; j++){
				fat_padre->type[j]=array_fat[i]->type[j];
			}*/
			copyArrayString(fat_padre->type, array_fat[i]->type);
			//strcpy(fat_padre->creator, array_fat[i]->creator);
			/*l=strlen(array_fat[i]->creator);
			for(int j=0; j<l; j++){
				fat_padre->creator[j]=array_fat[i]->creator[j];
			}*/
			copyArrayString(fat_padre->creator, array_fat[i]->creator);
			trovato=1;
		}else{
			i++;
		}
	}
	if(!trovato){
		printf("Chiusura del server in corso...\n");
		sendToServer(CLOSE_CMD);
		handle_error("Errore: impossibile recupeare la prima direcotry\n");
	}else{
		printf("Padre trovato\n");
	}
	
	current_path=NULL;
	currentPath();
	int wait_server=0;
	while(!e){
		printf("%s", CMD_LINE);
		fgets(row, 50, stdin);
		if(row[0]==10){
			fgets(row, 50, stdin);
		}
		//cmd=strtok(row, " \n");
		getCmd(cmd, row);
		if(strcmp(CREATE_FILE_CMD, cmd)==0){//GESTIRE IL PERCORSO E INODE PADRE
			printf("%sInserire il nome del file da creare senza spazi: ", CMD_LINE);
			//scanf("%s", info);
			fgets(info, 50, stdin);
			getCmd(name, info);
			//fflush(stdin);
			//printf("%s\n", name);
			while(name[0]==0){
				memset(info, 0, 50);
				memset(name, 0, 50);
				printf("%sInserire il nome del file da creare senza spazi: ", CMD_LINE);
				fgets(info, 50, stdin);
				getCmd(name, info);
			}
			sprintf(elem, "%s %s %s %s %s %d", CREAT_CMD, name, FILE_TYPE, user, current_path, fat_padre->inode);
			//printf("Stringa mandata %s\n", elem);
			sendToServer(elem);
			wait_server=1;
			//fflush(stdin);
		}else if(strcmp(ERASE_FILE_CMD, cmd)==0){//GESTIRE LA POSZIONE
			printf("%sInserire il nome del file da eliminare: ", CMD_LINE);
			fgets(info, 50, stdin);
			getCmd(name, info);
			while(name[0]==0){
				memset(info, 0, 50);
				memset(name, 0, 50);
				printf("%sInserire il nome del file da eliminare: ", CMD_LINE);
				fgets(info, 50, stdin);
				getCmd(name, info);
			}
			sprintf(elem, "%s %s %s %s", DELETE_CMD, name, FILE_TYPE, current_path);
			//printf("%s\n", elem);
			sendToServer(elem);
			wait_server=1;
			//fflush(stdin);
		}else if(strcmp(WRITE_FILE_CMD, cmd)==0){
			//printf("Write file digitato\n");
			printf("%sInserire il nome del file in cui scrivere: ", CMD_LINE);
			//scanf("%s", info);
			fgets(info, 50, stdin);
			getCmd(name, info);
			//fflush(stdin);
			int inode=searchElement(name, current_path);
			if(inode==0){
				printf("Errore: il file cercato non è presente nella directory corrente\n");
			}else{
				char* filename=(char*)malloc(sizeof(char)*(strlen(current_path)+strlen(name)+2));
				strcpy(filename, current_path);
				strcat(filename, name);
				//printf("%s\n", filename);
				int fd=open(filename, O_RDWR);
				if(fd==-1) handle_error("Errore: impossibile aprire il file in scrittura\n");
				printf("Apertura del file in corso...\n");
				
				printf("%sInserire da quale posizione si vuole iniziare la scrittura del file \n(digitare 0 se si vuole leggere dall'inizio): ", CMD_LINE);
				char el[64]={0};
				int position;
				//scanf("%d", &position);
				fgets(el, 64, stdin);
				if(el[0]==10){
					fgets(el, 64, stdin);
				}
				//printf("%s\n", el);
				char e[64]={0};
				
				getCmd(e, el);
				//printf("e: ");
				//for(int i=0; i<64; i++){
				//	printf("%c", e[i]);
				//}
				//printf("\n");
				position=strtol(e, NULL, 10);
				//printf("position %d\n", position);
				int a=isNumber(e);
				//printf("a %d\n", a);
				//fflush(stdin);
				while(!a || position<0){
					printf("Errore: impossibile effettuare il seeking del file\ncon il valore inserito in input\n");
					memset(el, 0, 64);
					memset(e, 0, 64);
					printf("%sInserire da quale posizione si vuole iniziare la scrittura del file \n(digitare 0 se si vuole leggere dall'inizio): ", CMD_LINE);
					fgets(el, 64, stdin);
					if(el[0]==10){
						fgets(el, 64, stdin);
					}
					getCmd(e, el);
					//printf("e: ");
					//for(int i=0; i<64; i++){
					//	printf("a %c", e[i]);
					//}
					//printf("\n");
					position=strtol(e, NULL, 10);
					a=isNumber(e);
					//printf("a %d\n", a);
				}
				
				if(position!=0){
					ret=lseek(fd, position, SEEK_SET);
					if(ret==-1){
						printf("Errore: impossibile spostare la posizione del file\n");
					}
				}
				
				char old_msg[1024]={0};
				ret=1;
				//printf("Contenuto del file '%s':\n", info);
				while(ret!=0 && ret!=-1){
					ret=read(fd, old_msg, 1024);
					//printf("elementi letti: %d\n", ret); 
					//printf("errno %d\n", errno);
					
				}
				if(ret==-1){
					printf("Errore: impossibile leggere il contenuto del file\n");
				}else{
					//printf("%s", old_msg);
				}
				ret=close(fd);
				if(ret==-1) handle_error("Errore: impossibile chiudere il file descriptor del file aperto in scrittura\n");
				
				fd=open(filename, O_WRONLY);
				if(fd==-1) handle_error("Errore: impossibile aprire il file in scrittura\n");
				ret=lseek(fd, position, SEEK_SET);
				if(ret==-1) printf("Errore: impossibile spostare la posizione del file\n");
				ret=close(fd);
				if(ret==-1) handle_error("Errore: impossibile chiudere il file descriptor del file aperto in scrittura\n");
				
				fd=open(filename, O_RDWR);
				if(fd==-1) handle_error("Errore: impossibile aprire il file in scrittura\n");
				ret=lseek(fd, position, SEEK_SET);
				if(ret==-1) printf("Errore: impossibile spostare la posizione del file\n");
				printf("%sScrivere il contenuto da inserire nel file:\n", CMD_LINE);
				char msg[1024]={0};
				//getchar();
				fgets(msg, 1024, stdin);
				//printf("Messaggio: ");
				//for(int i=0; i<strlen(msg); i++){
				//	printf("%c", msg[i]);
				//}
				//printf("\n");
				int wf=write(fd, msg, strlen(msg));
				//printf("Scritto lunghezza: %d\nInserito lunghezzza: %ld\n", wf, strlen(msg));
				if(wf==-1){
					printf("Errore: impossibile scrivere nel file\n");
				}else{
					printf("Scrittura avvenuta con successo\n");
				}
				wf=write(fd, old_msg, strlen(old_msg));
				//printf("Scritto lunghezza: %d\nInserito lunghezzza: %ld\n", wf, strlen(msg));
				if(wf==-1){
					printf("Errore: impossibile scrivere nel file\n");
				}else{
					//printf("Scrittura avvenuta con successo\n");
				}
				ret=close(fd);
				if(ret==-1) handle_error("Errore: impossibile chiudere il file descriptor del file aperto in scrittura\n");
				free(filename);
				memset(msg, 0, 1024);
				memset(old_msg, 0, 1024);
				//fflush(stdin);
			}
		}else if(strcmp(READ_FILE_CMD, cmd)==0){
			//printf("Read file digitato\n");
			printf("%sInserire il nome del file in cui scrivere: ", CMD_LINE);
			scanf("%s", info);
			//fflush(stdin);
			int inode=searchElement(info, current_path);
			//fflush(stdin);
			if(inode==0){
				printf("Errore: il file cercato non è presente nella directory corrente\n");
			}else{
				char* filename=(char*)malloc(sizeof(char)*(strlen(current_path)+strlen(info)+2));
				strcpy(filename, current_path);
				strcat(filename, info);
				//printf("%s\n", filename);
				int fd=open(filename, O_RDWR);
				if(fd==-1) handle_error("Errore: impossibile aprire il file in lettura\n");
				printf("Apertura del file in corso...\n");
				
				printf("%sInserire da quale posizione si vuole iniziare la lettura del file \n(digitare 0 se si vuole leggere dall'inizio): ", CMD_LINE);
				char el[64]={0};
				int position;
				//scanf("%d", &position);
				fgets(el, 64, stdin);
				if(el[0]==10){
					fgets(el, 64, stdin);
				}
				//printf("%s\n", el);
				char e[64]={0};
				
				getCmd(e, el);
				//printf("e: ");
				//for(int i=0; i<64; i++){
				//	printf("%c", e[i]);
				//}
				//printf("\n");
				position=strtol(e, NULL, 10);
				//printf("position %d\n", position);
				int a=isNumber(e);
				//printf("a %d\n", a);
				//fflush(stdin);
				while(!a || position<0){
					printf("Errore: impossibile effettuare il seeking del file\ncon il valore inserito in input\n");
					memset(el, 0, 64);
					memset(e, 0, 64);
					printf("%sInserire da quale posizione si vuole iniziare la lettura del file \n(digitare 0 se si vuole leggere dall'inizio): ", CMD_LINE);
					fgets(el, 64, stdin);
					if(el[0]==10){
						fgets(el, 64, stdin);
					}
					getCmd(e, el);
				//	printf("e: ");
				//	for(int i=0; i<64; i++){
				//		printf("a %c", e[i]);
				//	}
				//	printf("\n");
					position=strtol(e, NULL, 10);
					a=isNumber(e);
				//	printf("a %d\n", a);
				}
				if(position!=0){
					ret=lseek(fd, position, SEEK_SET);
					if(ret==-1){
						printf("Errore: impossibile spostare la posizione del file\n");
					}
				}
				
				char msg[1024]={0};
				ret=1;
				printf("Contenuto del file '%s':\n", info);
				while(ret!=0 && ret!=-1){
					ret=read(fd, msg, 1024);
					//printf("elementi letti: %d\n", ret); 
					//printf("errno %d\n", errno);
					
				}
				if(ret==-1){
					printf("Errore: impossibile leggere il contenuto del file\n");
				}else{
					printf("%s", msg);
				}
				ret=close(fd);
				if(ret==-1) handle_error("Errore: impossibile chiudere il file descriptor del file aperto in lettura\n");
				memset(msg, 0, 1024);
				free(filename);
			}
		}else if(strcmp(SEEK_FILE_CMD, cmd)==0){
			//printf("Seek file digitato\n");
			printf("Prima di eseguire la lettura o la scrittura di un file è possibile spostare la \nposizione corrente del file stesso, in modo tale che tali operazioni possano \navvenire nel punto desiderato\n");
			printf("NB: è obbligatorio inserire un valore\n");
		}else if(strcmp(CREATE_DIRECTORY_CMD, cmd)==0){//GESTIRE IL PERCORSO E INODE PADRE
			printf("%sInserire il nome della directory da creare senza spazi: ", CMD_LINE);
			fgets(info, 50, stdin);
			getCmd(name, info);
			while(name[0]==0){
				memset(info, 0, 50);
				memset(name, 0, 50);
				printf("%sInserire il nome della directory da creare senza spazi: ", CMD_LINE);
				fgets(info, 50, stdin);
				getCmd(name, info);
			}
			sprintf(elem, "%s %s %s %s %s %d", CREAT_CMD, name, DIR_TYPE, user, current_path, fat_padre->inode);
			sendToServer(elem);
			wait_server=1;
			//fflush(stdin);
		}else if(strcmp(ERASE_DIRECTORY_CMD, cmd)==0){//GESTIRE LA POSIZIONE
			printf("%sInserire il nome della directory da eliminare: ", CMD_LINE);
			fgets(info, 50, stdin);
			getCmd(name, info);
			while(name[0]==0){
				memset(info, 0, 50);
				memset(name, 0, 50);
				printf("%sInserire il nome della directory da eliminare: ", CMD_LINE);
				fgets(info, 50, stdin);
				getCmd(name, info);
			}
			sprintf(elem, "%s %s %s %s", DELETE_CMD, name, DIR_TYPE, current_path);
			//fflush(stdin);
			char risposta[50]={0};
			printf("%sEliminando la directory '%s' eliminerai tutto il suo contenuto.\nProseguire?[S/n]:", CMD_LINE, name);
			memset(info, 0, 50);
			fgets(info, 50, stdin);
			getCmd(risposta, info);
			while(risposta[0]==0){
				memset(info, 0, 50);
				memset(risposta, 0, 50);
				printf("Proseguire?[S/n]:");
				fgets(info, 50, stdin);
				getCmd(risposta, info);
			}
			if(strcmp(risposta,"S")==0){
				printf("Selezionato si\n");
				sendToServer(elem);
				wait_server=1;
			}else{
				printf("Selezionato no o carattere diverso\n");
				//fflush(stdin);
			}
		}else if(strcmp(CHANGE_DIRECTORY_CMD, cmd)==0){
			//printf("Change directory digitato\n");
			printf("%sInserire il nome della directory in cui spostarsi: ", CMD_LINE);
			scanf("%s", info);
			//fflush(stdin);
			if(strcmp(MOVE_FATHER, info)==0){
				if(strcmp(FILE_SYSTEM_DIRECTORY, fat_padre->name)==0){
					printf("Errore: impossibile spostarsi nella direcotry padre di %s\n", FILE_SYSTEM_DIRECTORY);
				}else{
					int padre=fat_padre->inode_padre;
					fat_padre->inode=array_fat[padre]->inode;
					//fat_padre->size=array_fat[padre]->size;
					fat_padre->inode_padre=array_fat[padre]->inode_padre;
					strcpy(fat_padre->name, array_fat[padre]->name);
					strcpy(fat_padre->path, array_fat[padre]->path);
					strcpy(fat_padre->type, array_fat[padre]->type);
					strcpy(fat_padre->creator, array_fat[padre]->creator);
					currentPath();
				}
			}else{
				int inode=INODE_LIMIT;
				int trovato=0;
				for(int i=0; i<MAX_INODE && !trovato; i++){
					if(array_fat[i]!=NULL){
						if(strcmp(info, array_fat[i]->name)==0){
							trovato=1;
							inode=array_fat[i]->inode;
						}
					}
				}
				if(trovato){
					fat_padre->inode=array_fat[inode]->inode;
					//fat_padre->size=array_fat[inode]->size;
					fat_padre->inode_padre=array_fat[inode]->inode_padre;
					strcpy(fat_padre->name, array_fat[inode]->name);
					strcpy(fat_padre->path, array_fat[inode]->path);
					strcpy(fat_padre->type, array_fat[inode]->type);
					strcpy(fat_padre->creator, array_fat[inode]->creator);
					currentPath();
				}else{
					printf("Errore: la directory %s non è presente nella directory corrente\n", info);
					printf("Si consiglia di eseguire ld (list direcotry) per visualizzare gli elementi \npresenti nella directory corrente\n");
				}
			}
		}else if(strcmp(LIST_DIRECTORY_CMD, cmd)==0){
			//printf("List directory digitato\n");
			printf("Directory corrente: %s%s/\n", fat_padre->path, fat_padre->name);
			printf("Nome\t\tTipo\n");
			for(int i=0; i<MAX_INODE; i++){
				if(array_fat[i]!=NULL){
					//printf("inode %d padre %d", array_fat[i]->inode_padre, fat_padre->inode);
					if(array_fat[i]->inode_padre==fat_padre->inode){
						printf("%s\t\t%s\n", array_fat[i]->name, array_fat[i]->type);
					}
				}
			}
		}else if(strcmp(HELP_CMD, cmd)==0){
			printInfo();
			printInfoMD();
		}else if(strcmp(EXIT_CMD, cmd)==0){
			e=1;
			printf("Uscendo dal sistema...\n");
		}else if(strcmp(CLOSE_CMD, cmd)==0){
			e=1;
			sprintf(elem, "%s", CLOSE_CMD);
			printf("Chiusura del server in corso...\n");
			sendToServer(elem);
			printf("Uscendo dal sistema...\n");
		}else{
			printf("Errore: impossibile riconoscere il comando '%s'\n", cmd);
			printf("Usa il comando '%s'per visualizzare i possibili comandi da utilizzare\n", HELP_CMD);
		}
		
		if(wait_server){
			wait_server=0;
			ret=sem_wait(main_s);
			if(ret==-1) handle_error("Errore: sem_wait server\n");
			ret=sem_wait(shmem);
			if(ret==-1) handle_error("Errore: sem_wait shmem\n");
			sharing_father();
			ret=sem_post(shmem);
			if(ret==-1) handle_error("Errore: sem_post shmem\n");
			ret=sem_post(server);
			if(ret==-1) handle_error("Errore: sem_post main	\n");
			//stampaArray();
		}
		//while(getchar()!='\n');
		//memset(row, 0, 50);
		//row={0};
		memset(cmd, 0, 50);
		memset(info, 0, 50);
		memset(name, 0, 50);
		memset(elem, 0, 100);
		//fflush(stdin);
		//fflush(stdout);
	}
	ret=sem_close(server);
	if(ret==-1) handle_error("Errore: sem_close server\n");
	ret=sem_close(shmem);
	if(ret==-1) handle_error("Errore: sem_close shmem\n");
	ret=sem_close(main_s);
	if(ret==-1) handle_error("Errore: sem_close main\n");
	printf("Grazie per aver lavorato con noi\n");
	
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			free(array_fat[i]);
		}
	}
	free(array_fat);
	free(fat_padre);
	free(current_path);
	//free(cmd);
	
	ret=unlink(FIFO_FOR_RES);
	if(ret==-1) handle_error("Errore: impossibile eliminare il canale di comunicazione\n");
	exit(1);
}
