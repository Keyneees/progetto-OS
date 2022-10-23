#define CMD_LINE "OS PROJECT> "
#define CREATE_FILE_CMD "cf"
#define ERASE_FILE_CMD "ef"
#define WRITE_FILE_CMD "wf"
#define READ_FILE_CMD "rf"
#define SEEK_FILE_CMD "sf"
#define CREATE_DIRECTORY_CMD "cd"
#define ERASE_DIRECTORY_CMD "ed"
#define CHANGE_DIRECTORY_CMD "md"
#define LIST_DIRECTORY_CMD "ld"
#define HELP_CMD "help"
#define EXIT_CMD "exit"
#define MOVE_FATHER ".."

#define handle_error(msg)           do { printf(msg); exit(EXIT_FAILURE); } while (0) //RISCRIVI FATTA BENE

#include <stdlib.h>
#include <stdio.h>

int inode_padre;
char user[30];
struct fat* fat_padre;

void printInfo();
void printInfoMD();
void sharing_father();
void sendToServer(char* elem);
void waitResult();
void stampaArray(); //DA CANCELLARE
void currentPath();
int searchElement(char* name, char* path);
//int compareArrayString(char elem[64], char* cmp);
void compareArrayString(char elem[64], char copy[64]);
