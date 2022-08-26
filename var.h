#define MAX_INODE 128
#define MAX_LENGTH_FILE_NAME 32
#define SERVER_FATHER -1

#define FAT_FILE_NAME "FAT.txt"
#define FILE_SYSTEM_DIRECTORY "file_system"
#define GENERIC_CREATOR "server"

#define FILE_TYPE 'F'
#define DIR_TYPE 'D'

#define FIFO_FOR_FAT "/sharedfifo"

#define DELETE_CMD "delete"
#define UPDATE_CMD "update"
#define SEPARATOR " \n"

#define PERMESSI_FILE 0666
#define PERMESSI_DIRECTORY 0777
#define INODE_LIMIT 999

int next_inode;
char* current_path;
/*
struct FAT{
	int inode;
	int size;
	char* creator;
};

struct file_struct{
	struct FAT* fat;
	char* name;
	char* path;
};

struct list_file{
	struct file_struct* elem;
	struct list_file* next;
};

struct directory_struct{
	struct file_struct file;
	struct list_file* list;
};

struct FAT* array_fat[MAX_INODE];*/

struct sfile{
	char* nome;
	char* path;
	char type;
};

struct sdirectory{
	struct sfile* sfile;
	int* element;
};

struct fat{
	int inode;
	int size;
	char* creator;
	struct sdirectory* sfile;
	int inode_padre;
};

struct fat* array_fat[MAX_INODE];
struct fat* fat_padre;

