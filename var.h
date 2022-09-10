#define MAX_INODE 128
#define MAX_LENGTH_FILE_NAME 32
#define SERVER_FATHER -1

#define FAT_FILE_NAME "FAT.txt"
#define FILE_SYSTEM_DIRECTORY "file_system"
#define GENERIC_CREATOR "server"

#define FILE_TYPE "F"
#define DIR_TYPE "D"

#define FIFO_FOR_FAT "infofat"
#define FIFO_FOR_RES "inforesult"
#define SHMEM_FOR_INFO "/infomem"

#define CREAT_CMD "creat"
#define DELETE_CMD "delete"
#define UPDATE_CMD "update"
#define CLOSE_CMD "close"
#define SEPARATOR " \n"

#define PERMESSI_FILE 0666
#define PERMESSI_DIRECTORY 0777
#define INODE_LIMIT 999

int next_inode;
char* current_path;

struct fat{
	int inode;
	char name[64];
	char path[64];
	char type[64];
	int size; //DA VALUTARE
	int inode_padre;
	char creator[64];
};

struct fat* array_fat[MAX_INODE];
struct fat* fat_padre;

