#define AUX "appoggio.txt"

void nextInode();
void insertInFatFile(char *row, int inode);
void loadFAT();
void updateFat(int inode, int size);
void sharing_father();
void stampaArray();
void searchElement(char* name, char* size, char* type);
