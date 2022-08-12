#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


#define handle_error(msg)           do { printf(msg); exit(EXIT_FAILURE); } while (0)

//FUNZIONI DA IMPLEMENTARE PER I FILE
void createFile(int fd, int inode, char* filename, char type, char* creator);
void eraseFile(char* filename);
void writeFile();
void readFile();
void seekFile();

//FUNZIONI DA IMPLEMENTARE PER LE DIRECTORY
void createDirectory(int fd, int inode, char* directoryname, char type, char* creator);
void eraseDirectory(char* directoryname);
void changeDirectory();
void listDirectory();

//FUNZIONI EXTRA
int nextInode(int fdfile);
void addElement(int fd, char* elem, int size_elem);
void insertInFatFile(int fd, int inode, char* name, char type, char* creator);
