#include "fun.h"
#include "var.h"
#include "fun_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

char* current_path;
int inode_padre;
char user[30];
sem_t* server;
sem_t* shmem;
sem_t* main_s;

int main(){
	array_fat=(struct fat**)malloc(sizeof(struct fat*));
	
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
	
	for(int i=0; i<MAX_INODE; i++){
		array_fat[i]=(struct fat*)malloc(sizeof(struct fat));	
		array_fat[i]=NULL;
	}
	printf("%sInserire il nome per il login: ", CMD_LINE);
	scanf("%s", user);
	while(strcmp(GENERIC_CREATOR, user)==0){
		printf("Errore: non è possibile usare il nome %s. Si prega di inserire un nome diverso\n", user);
		printf("%sInserire il nome per il login: ", CMD_LINE);
		scanf("%s", user);
	}
	printf("Benvenuto %s nel sistema FAT_OS\n", user);
	/**/
	printInfo();
	
	char cmd[50];
	char info[50];
	char elem[100];
	int e=0;
	int ret;
	sharing_father();
	stampaArray();
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
		if(strcmp(array_fat[i]->name, FILE_SYSTEM_DIRECTORY)==0){
			fat_padre=(struct fat*)malloc(sizeof(struct fat));
			fat_padre->inode=array_fat[i]->inode;
			fat_padre->size=array_fat[i]->size;
			fat_padre->inode_padre=array_fat[i]->inode_padre;
			strcpy(fat_padre->name, array_fat[i]->name);
			strcpy(fat_padre->path, array_fat[i]->path);
			strcpy(fat_padre->type, array_fat[i]->type);
			strcpy(fat_padre->creator, array_fat[i]->creator);
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
	unlink(FIFO_FOR_RES);
	currentPath();
	int wait_server=0;
	while(!e){
		printf("%s", CMD_LINE);
		scanf("%s", cmd);
		printf("Comando inserito: %s\n", cmd);
		if(strcmp(CREATE_FILE_CMD, cmd)==0){//GESTIRE IL PERCORSO E INODE PADRE
			printf("%sInserire il nome del file da creare: ", CMD_LINE);
			scanf("%s", info);
			sprintf(elem, "%s %s %s %s %s %d", CREAT_CMD, info, FILE_TYPE, user, current_path, fat_padre->inode);
			printf("Stringa mandata %s\n", elem);
			sendToServer(elem);
			wait_server=1;
		}else if(strcmp(ERASE_FILE_CMD, cmd)==0){//GESTIRE LA POSZIONE
			printf("%sInserire il nome del file da eliminare: ", CMD_LINE);
			scanf("%s", info);
			sprintf(elem, "%s %s %s %s", DELETE_CMD, info, FILE_TYPE, current_path);
			printf("%s\n", elem);
			sendToServer(elem);
			wait_server=1;
		}else if(strcmp(WRITE_FILE_CMD, cmd)==0){
			printf("Write file digitato\n");
		}else if(strcmp(READ_FILE_CMD, cmd)==0){
			printf("Read file digitato\n");
		}else if(strcmp(SEEK_FILE_CMD, cmd)==0){
			printf("Seek file digitato\n");
		}else if(strcmp(CREATE_DIRECTORY_CMD, cmd)==0){//GESTIRE IL PERCORSO E INODE PADRE
			printf("%sInserire il nome della directory da creare: ", CMD_LINE);
			scanf("%s", info);
			sprintf(elem, "%s %s %s %s %s %d", CREAT_CMD, info, DIR_TYPE, user, current_path, fat_padre->inode);
			sendToServer(elem);
			wait_server=1;
		}else if(strcmp(ERASE_DIRECTORY_CMD, cmd)==0){//GESTIRE LA POSIZIONE
			printf("%sInserire il nome della directory da eliminare: ", CMD_LINE);
			scanf("%s", info);
			sprintf(elem, "%s %s %s %s", DELETE_CMD, info, DIR_TYPE, current_path);
			char risposta[1];
			printf("%sEliminando la directory '%s' eliminerai tutto il suo contenuto. Proseguire?[S/n]", CMD_LINE, info);
			scanf("%s", risposta);
			if(strcmp(risposta,"S")==0){
				printf("Selezionato si\n");
				sendToServer(elem);
			}else{
				printf("Selezionato no o carattere diverso\n");
			}
			wait_server=1;
		}else if(strcmp(CHANGE_DIRECTORY_CMD, cmd)==0){
			printf("Change directory digitato\n");
			printf("%sInserire il nome della directory in cui spostarsi: ", CMD_LINE);
			scanf("%s", info);
			if(strcmp(MOVE_FATHER, info)==0){
				if(strcmp(FILE_SYSTEM_DIRECTORY, fat_padre->name)==0){
					printf("Errore: impossibile spostarsi nella direcotry padre di %s\n", FILE_SYSTEM_DIRECTORY);
				}else{
					int padre=fat_padre->inode_padre;
					fat_padre->inode=array_fat[padre]->inode;
					fat_padre->size=array_fat[padre]->size;
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
					fat_padre->size=array_fat[inode]->size;
					fat_padre->inode_padre=array_fat[inode]->inode_padre;
					strcpy(fat_padre->name, array_fat[inode]->name);
					strcpy(fat_padre->path, array_fat[inode]->path);
					strcpy(fat_padre->type, array_fat[inode]->type);
					strcpy(fat_padre->creator, array_fat[inode]->creator);
					currentPath();
				}else{
					printf("Errore: la directory %s non è presente nella directory corrente\n", info);
					printf("Si consiglia di eseguire ld (list direcotry) per visualizzare gli elementi presenti nella directory corrente\n");
				}
			}
		}else if(strcmp(LIST_DIRECTORY_CMD, cmd)==0){
			printf("List directory digitato\n");
			printf("Directory corrente: %s%s/\n", fat_padre->path, fat_padre->name);
			printf("Nome\tTipo\n");
			for(int i=0; i<MAX_INODE; i++){
				if(array_fat[i]!=NULL){
					if(array_fat[i]->inode_padre==fat_padre->inode){
						printf("%s\t%s\n", array_fat[i]->name, array_fat[i]->type);
					}
				}
			}
		}else if(strcmp(HELP_CMD, cmd)==0){
			printInfo();
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
			stampaArray();
		}
		
		memset(cmd, 0, 50);
		memset(info, 0, 50);
		memset(elem, 0, 100);
	}
	printf("Grazie per aver lavorato con noi\n");
	exit(1);
}
