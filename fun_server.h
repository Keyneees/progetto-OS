#define AUX "appoggio.txt"

void nextInode(int fdfile);
void insertInFatFile(char *row, int inode);
void loadFAT();
void updateFat(int inode, int size);
