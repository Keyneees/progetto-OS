#define MAX_INODE 128
#define MAX_LENGTH_FILE_NAME 32

#define FAT_FILE_NAME "FAT.txt"
#define FILE_SYSTEM_DIRECTORY "file system"
#define GENERIC_CREATOR "server"

#define FILE_TYPE 'F'
#define DIR_TYPE 'D'

#define FIFO_FOR_FAT "/sharedfifo"

#define DELETE_CMD "delete"
#define SEPARATOR " "

int next_inode;
char* current_path;

struct FAT{
	int inode;
	int size;
	char* creator;
}FAT;

struct file_struct{
	struct FAT* fat;
	char* name;
	struct directory_struct* directory;
	char* path;
}file_struct;

struct list_file{
	struct file_struct* elem;
	struct list_file* next;
}list_file;

struct directory_struct{
	struct file_struct file;
	struct list_file* list;
}directory_struct;

struct FAT* array_fat[MAX_INODE];
struct directory_struct* current_directory;

