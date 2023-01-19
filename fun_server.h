#define AUX "appoggio.txt"
#define handle_error(msg)           do { printf(msg); exit(EXIT_FAILURE); } while (0) //RISCRIVI FATTA BENE

#include <stdlib.h>
#include <stdio.h>

#define FAT_FILE_NAME "FAT.txt"
#define SERVER_FATHER -1
#define SEPARATOR " \n"
#define PERMESSI_FILE 0666
#define PERMESSI_DIRECTORY 0777

int inode_fat;
int inode_dir;
int fdfat;
char buf[100];

//FUNZIONI DA IMPLEMENTARE PER I FILE
void createFile(int inode, char* filename, char* type, char* creator, char* path, int inode_padre);
void eraseFile(int inode);

//FUNZIONI DA IMPLEMENTARE PER LE DIRECTORY
void createDirectory(int inode, char* directoryname, char* type, char* creator, char* path, int inode_padre);
void eraseDirectory(int inode);

//FUNZIONI EXTRA
void removeChild(int inode);void nextInode();
void insertInFatFile(char *row, int inode);
void loadFAT();
void updateFat(int inode, int size);
void sharing_father();
void stampaArray();
int searchElement(char* name, char* size, char type);
void sendResult();
