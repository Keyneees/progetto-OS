#include <stdio.h>

#define handle_error(msg)           do { printf(msg); exit(EXIT_FAILURE); } while (0)

//FUNZIONI DA IMPLEMENTARE PER I FILE
void createFile(char* creator, char* filename);
void eraseFile(char* filename);
void writeFile();
void readFile();
void seekFile();

//FUNZIONI DA IMPLEMENTARE PER LE DIRECTORY
void createDirectory(char* creator, char* directoryname);
void eraseDirectory(char* directoryname);
void changeDirectory();
void listDirectory();

//FUNZIONI EXTRA
void rowCounter(int fdfile);
void addElement(int fd, char* row);
