#include "fun.h"
#include "var.h"
#include "fun_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* current_path;
char user[30];

int main(){
	printf("%sInserire il nome per il login: ", CMD_LINE);
	scanf("%s", user);
	while(strcmp(GENERIC_CREATOR, user)==0){
		printf("Errore: non è possibile usare il nome %s. Si prega di inserire un nome diverso\n", user);
		printf("%s", CMD_LINE);
		scanf("%s", user);
	}
	printf("Benvenuto %s nel sistema FAT_OS\n", user);
	printInfo();
	
	char cmd[50];
	char info[50];
	int e=0;
	while(!e){
		printf("%s", CMD_LINE);
		scanf("%s", cmd);
		printf("Comando inserito: %s\n", cmd);
		if(strcmp(CREATE_FILE_CMD, cmd)==0){
			printf("%sInserire il nome del file da creare: ", CMD_LINE);
			scanf("%s", info);
			createFile(next_inode, info, FILE_TYPE, user);
		}else if(strcmp(ERASE_FILE_CMD, cmd)==0){
			printf("Erase file digitato\n");
		}else if(strcmp(WRITE_FILE_CMD, cmd)==0){
			printf("Write file digitato\n");
		}else if(strcmp(READ_FILE_CMD, cmd)==0){
			printf("Read file digitato\n");
		}else if(strcmp(SEEK_FILE_CMD, cmd)==0){
			printf("Seek file digitato\n");
		}else if(strcmp(CREATE_DIRECTORY_CMD, cmd)==0){
			printf("%sInserire il nome della directory da creare: ", CMD_LINE);
			scanf("%s", info);
			createDirectory(next_inode, info, DIR_TYPE, user);
		}else if(strcmp(ERASE_DIRECTORY_CMD, cmd)==0){
			printf("Erase directory digitato\n");
		}else if(strcmp(CHANGE_DIRECTORY_CMD, cmd)==0){
			printf("Change directory digitato\n");
		}else if(strcmp(LIST_DIRECTORY_CMD, cmd)==0){
			printf("List directory digitato\n");
		}else if(strcmp(HELP_CMD, cmd)==0){
			printInfo();
		}else if(strcmp(EXIT_CMD, cmd)==0){
			e=1;
			printf("Uscendo dal sistema...\n");
		}else{
			printf("Errore: impossibile riconoscere il comando '%s'\n", cmd);
			printf("Usa il comando '%s'per visualizzare i possibili comandi da utilizzare\n", HELP_CMD);
		}
	}
	printf("Grazie per aver lavorato con noi\n");
	exit(1);
}
