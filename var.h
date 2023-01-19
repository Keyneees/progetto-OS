#define MAX_INODE 128

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

#define INODE_LIMIT 999

#define SEM_SERVER "/server\0"
#define SEM_MAIN "/main\0"
#define SEM_SHMEM "/shmem\0"

#include <semaphore.h>

struct fat{
	int inode;
	char name[64];
	char path[64];
	char type[64];
	int inode_padre;
	char creator[64];
};

struct fat** array_fat;
int next_inode;
char* current_path;
sem_t* server;
sem_t* shmem;
sem_t* main_s;

