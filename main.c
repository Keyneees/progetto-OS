#include "fun.h"
#include "var.h"
#include "fun_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* current_path;
int inode_padre;
char user[30];

int main(){
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
	sharing_father();
	inode_padre=fat_padre->inode;
	printf("Info padre:\n");
	printf("Puntantore: %p\n", fat_padre);
	printf("Puntatore nome %p\n", fat_padre->name);
	printf("inode %d\n", fat_padre->inode);
	printf("nome %s\n", fat_padre->name);
	printf("path %s\n", fat_padre->path);
	unlink(FIFO_FOR_RES);
	if(strcmp(fat_padre->path, "/")==0){
		printf("Percorso trovato: /\n");
		current_path=(char*)malloc(sizeof(char)*(strlen(fat_padre->name)+3));
		current_path=strcpy(current_path, fat_padre->name);
		current_path=strcat(current_path, "/");
	}else{
		printf("Percorso non basilare\n");
		current_path=(char*)malloc(sizeof(char)*(strlen(fat_padre->path)+strlen(fat_padre->name)+3));
		current_path=strcpy(current_path, fat_padre->path);
		current_path=strcat(current_path, fat_padre->name);
		current_path=strcat(current_path, "/");
	}
	while(!e){
		printf("%s", CMD_LINE);
		scanf("%s", cmd);
		printf("Comando inserito: %s\n", cmd);
		if(strcmp(CREATE_FILE_CMD, cmd)==0){//GESTIRE IL PERCORSO E INODE PADRE
			printf("%sInserire il nome del file da creare: ", CMD_LINE);
			scanf("%s", info);
			sprintf(elem, "%s %s %s %s %s %d", CREAT_CMD, info, FILE_TYPE, user, current_path, inode_padre);
			printf("Stringa mandata %s\n", elem);
			sendToServer(elem);
		}else if(strcmp(ERASE_FILE_CMD, cmd)==0){//GESTIRE LA POSZIONE
			printf("%sInserire il nome del file da eliminare: ", CMD_LINE);
			scanf("%s", info);
			sprintf(elem, "%s %s %s %s", DELETE_CMD, info, FILE_TYPE, current_path);
			printf("%s\n", elem);
			sendToServer(elem);
		}else if(strcmp(WRITE_FILE_CMD, cmd)==0){
			printf("Write file digitato\n");
		}else if(strcmp(READ_FILE_CMD, cmd)==0){
			printf("Read file digitato\n");
		}else if(strcmp(SEEK_FILE_CMD, cmd)==0){
			printf("Seek file digitato\n");
		}else if(strcmp(CREATE_DIRECTORY_CMD, cmd)==0){//GESTIRE IL PERCORSO E INODE PADRE
			printf("%sInserire il nome della directory da creare: ", CMD_LINE);
			scanf("%s", info);
			sprintf(elem, "%s %s %s %s %s %d", CREAT_CMD, info, DIR_TYPE, user, current_path, inode_padre);
			sendToServer(elem);
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
		}else if(strcmp(CHANGE_DIRECTORY_CMD, cmd)==0){
			printf("Change directory digitato\n");
		}else if(strcmp(LIST_DIRECTORY_CMD, cmd)==0){
			printf("List directory digitato\n");
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
		memset(cmd, 0, 50);
		memset(info, 0, 50);
		memset(elem, 0, 100);
	}
	printf("Grazie per aver lavorato con noi\n");
	exit(1);
}
