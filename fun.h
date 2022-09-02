#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


#define handle_error(msg)           do { printf(msg); exit(EXIT_FAILURE); } while (0) //RISCRIVI FATTA BENE

//FUNZIONI DA IMPLEMENTARE PER I FILE
void createFile(int inode, char* filename, char* type, char* creator, char* path, int inode_padre);
void eraseFile(int inode);
void writeFile();
void readFile();
void seekFile();

//FUNZIONI DA IMPLEMENTARE PER LE DIRECTORY
void createDirectory(int inode, char* directoryname, char* type, char* creator, char* path, int inode_padre);
void eraseDirectory(int inode);
void changeDirectory();
void listDirectory();

//FUNZIONI EXTRA
void addElement(int fd, char* elem, int size_elem);
void sizeUpdate(int inode);
