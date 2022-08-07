#define MAX_INODE 128
#define MAX_LENGTH_FILE_NAME 32

#define FAT_FILE_NAME "FAT.txt"
#define FILE_SYSTEM_DIRECTORY "file system"
#define GENERIC_CREATOR "server"

#define FILE_TYPE "F"
#define DIR_TYPE "D"

#define FIFO_FOR_FAT "/sharedfifo"

#define DELETE_CMD "delete"
#define SEPARATOR " "

int next_inode;
char* current_directory; //VEDERE SE SERVE METTERE DIRETTAMENTE LA STRUTTURA DELLA DIRECTORY CORRENTE

struct FAT{
	int inode;
	ssize_t size;
	char creator[MAX_LENGTH_FILE_NAME];
}FAT;

struct file_struct{
	FAT fat;
	char name[MAX_LENGTH_FILE_NAME];
	directory_struct* directory;
}file_struct;

struct list_file{
	file_struct* elem;
	list_file* next;
}list_file;

struct directory_struct{
	file_struct file;
	list_file* list;
}directory_struct;

FAT array_fat[MAX_INODE];

