#include "fun_server.h"
#include "var.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


//FUNZONI PER I FILE
void createFile(int inode, char* filename, char* type, char* creator, char* path, int inode_padre){
	int ret;
	char res[128]={0};
	memset(res, 0, 128);
	if(next_inode<MAX_INODE){
		char* location;
		if(strcmp(path, "/")==0){
			location=(char*)malloc(sizeof(char)*(strlen(filename)));
			strcpy(location, filename);
		}else{
			location=(char*)malloc(sizeof(char)*(strlen(path)+strlen(filename)+2));
			strcpy(location, path);
			strcat(location, filename);
		}
		int create=open(location, O_CREAT | O_EXCL, PERMESSI_FILE);
		free(location);
		if(create==-1){
			if(errno==EEXIST){
				sprintf(res,"Errore: impossibile creare il file: nome già in uso");
				printf("%s", res);
			}else{
				sprintf(res, "Errore: impossbile creare il file");
				printf("%s", res);
			}
		}else{
			printf("File %s creato con successo\n", filename);
			printf("Creazione della struttura\n");
			array_fat[next_inode]=(struct fat*)calloc(1, sizeof(struct fat));
			array_fat[next_inode]->inode=next_inode;
			//array_fat[next_inode]->name=(char*)malloc(sizeof(char)*strlen(filename));
			strcpy(array_fat[next_inode]->name, filename);
			//array_fat[next_inode]->path=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->path, path);
			//array_fat[next_inode]->type=(char*)malloc(sizeof(char));
			strcpy(array_fat[next_inode]->type, type);
			//array_fat[next_inode]->type=type;
			array_fat[next_inode]->size=0;
			//array_fat[next_inode]->creator=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->creator, creator);
			array_fat[next_inode]->inode_padre=inode_padre;
			sprintf(res, "Elemento creato con successo");
			printf("%s", res);
			char row[100]={0};
			sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, filename, path, type, creator, 0, inode_padre);
			insertInFatFile(row, next_inode);
			printf("Fat file aggiornato\n");
		}
	}else{
		//GESTIONE IMPOSSIBLITA' DI CREARE IL FILE
		sprintf(res, "Impossibile creare il nuovo file, limite di file disponibili raggiunto\nModifica o elimina file/directory già esistenti\n");
		printf("%s", res);
	}
	printf("Fine preparazione fat_element\n");
	int dif=strcmp(GENERIC_CREATOR, creator);
	if(dif){
		printf("Messaggio mandato al server: %s\n", res);
		sendResult(res);
	}
}

void eraseFile(int inode){
	char* filename=(char*)malloc(sizeof(char)*(strlen(array_fat[inode]->path)+strlen(array_fat[inode]->name)+2));
	strcpy(filename, array_fat[inode]->path);
	strcat(filename, array_fat[inode]->name);
	int ret=remove(filename);
	free(filename);
	if(ret==-1){
		printf("Il file %s non esiste nella directory corrente\n", filename);
	}else{
		/*free(array_fat[inode]->name);
		free(array_fat[inode]->path);
		free(array_fat[inode]->type);
		free(array_fat[inode]->creator);*/
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		
		/*char elem[100];
		printf("Preparo la stringa\n");
		sprintf(elem, "%s %d\n", DELETE_CMD, inode);
		printf("Stringa: %s\n", elem);
		sendToServer(elem);*/
		char in[10]={0};
		sprintf(in, "%d\n", inode);
		insertInFatFile(in, inode);
	}
	
}

//FUNZIONI PER LE DIRECTORY
void createDirectory(int inode, char* directoryname, char* type, char* creator, char* path, int inode_padre){
	char res[128]={0};
	if(next_inode<MAX_INODE){
		char* location;
		if(strcmp(path, "/")==0){
			location=(char*)malloc(sizeof(char)*(strlen(directoryname)));
			strcpy(location, directoryname);
		}else{
			location=(char*)malloc(sizeof(char)*(strlen(path)+strlen(directoryname)+2));
			strcpy(location, path);
			strcat(location, directoryname);
		}
		printf("Elementi dati -> path %s, nome %s", path, directoryname);
		printf("Location calcolata %s\n", location);
		int fddir=mkdir(location, PERMESSI_DIRECTORY);
		free(location);
		if(fddir==-1){
			if(errno==EEXIST){
				sprintf(res,"Errore: impossibile creare la directory: nome già in uso");
				printf("%s", res);
			}else{
				sprintf(res, "Errore: impossbile creare la directory");
				printf("%s", res);
			}
		}else{
			array_fat[next_inode]=(struct fat*)calloc(1, sizeof(struct fat));
			array_fat[next_inode]->inode=inode;
			//array_fat[next_inode]->name=(char*)malloc(sizeof(char)*strlen(directoryname));
			strcpy(array_fat[next_inode]->name, directoryname);
			//array_fat[next_inode]->path=(char*)malloc(sizeof(char)*strlen(current_path));
			strcpy(array_fat[next_inode]->path, path);
			//array_fat[next_inode]->type=(char*)malloc(sizeof(char));
			strcpy(array_fat[next_inode]->type, type);
			//array_fat[next_inode]->type=type;
			array_fat[next_inode]->size=0;
			//array_fat[next_inode]->creator=(char*)malloc(sizeof(char)*strlen(creator));
			strcpy(array_fat[next_inode]->creator, creator);
			array_fat[next_inode]->inode_padre=inode_padre;
			sprintf(res, "Elemento creato con successo");
			printf("Elemento inserito\n");
			char row[100]={0};
			sprintf(row, "%d %s %s %s %s %d %d\n", next_inode, directoryname, path, type, creator, 0, inode_padre);
			insertInFatFile(row, next_inode);
		}
	}else{
		//GESTIONE IMPOSSIBILITA' DI CREARE LA DIRECTORY
		sprintf(res, "Impossibile creare la nuova directory, limite di file disponibili raggiunto\nModifica o elimina file/directory già esistenti\n");
		printf("%s", res);
		
	}
	if(strcmp(creator, GENERIC_CREATOR)!=0){
		//sleep(3);
		printf("Messaggio mandato al server: %s\n", res);
		sendResult(res);
	}
}

void eraseDirectory(int inode){
	removeChild(inode);
	char* directoryname=(char*)malloc(sizeof(char)*(strlen(array_fat[inode]->path)+strlen(array_fat[inode]->name)+2));
	strcpy(directoryname, array_fat[inode]->path);
	strcat(directoryname, array_fat[inode]->name);
	int ret=remove(directoryname);
	printf("errno %d\n", errno);
	if(ret==-1){
		printf("La directory %s non esiste nella directory corrente\n", directoryname);
	}else{
		
		/*free(array_fat[inode]->name);
		free(array_fat[inode]->path);
		free(array_fat[inode]->type);
		free(array_fat[inode]->creator);*/
		free(array_fat[inode]);
		array_fat[inode]=NULL;
		
		/*char elem[100];
		printf("Preparo la stringa\n");
		sprintf(elem, "%s %d\n", DELETE_CMD, inode);
		printf("Stringa: %s\n", elem);
		sendToServer(elem);*/
		char in[10]={0};
		sprintf(in, "%d\n", inode);
		insertInFatFile(in, inode);
	}
	free(directoryname);
}



//FUNZIONI EXTRA

void sizeUpdate(int inode){
	if(inode>=0 && inode<MAX_INODE){
		struct stat s;
		char* filename=array_fat[inode]->name;
		printf("Pronto ad aggiornare la size del file %s\n", filename);
		int ret=stat(filename, &s);
		printf("Fatto...");
		printf("Dimensione dell'elemento di inode %d: %ld\n", inode, s.st_size);
		array_fat[inode]->size=s.st_size;
		
		if(strcmp(array_fat[inode]->creator,GENERIC_CREATOR)){
			/*char elem[100];
			sprintf(elem, "%s %d %s %s %s %s %d %d\n", UPDATE_CMD, inode, array_fat[inode]->name, 
			array_fat[inode]->path, array_fat[inode]->type, array_fat[inode]->creator, array_fat[inode]->size, 
			array_fat[inode]->inode_padre);
			sendToServer(elem);*/
		}else{
			char elem[sizeof(struct fat)+64]={0};
			sprintf(elem, "%d %s %s %s %s %d %d\n", inode, array_fat[inode]->name, 
			array_fat[inode]->path, array_fat[inode]->type, array_fat[inode]->creator, array_fat[inode]->size, 
			array_fat[inode]->inode_padre);
			insertInFatFile(elem, inode);
		}
		sizeUpdate(array_fat[inode]->inode_padre);
	}
}

void removeChild(int inode){
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			if(array_fat[i]->inode_padre==inode){
				if(strcmp(array_fat[i]->type, DIR_TYPE)==0){
					removeChild(i);
					eraseDirectory(i);
				}else{
					eraseFile(i);
				}
				//free(array_fat[inode]);
				//array_fat[inode]=NULL;
			}
		}
	}
}





void nextInode(){  //DA RIVEDERE
	int trovato=0;
	int i=0;
	char buffer[80]={0};
	int ret=0;
	FILE* fdfat=fopen(FAT_FILE_NAME, "r");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire il file in lettura");
	while(buffer[0]!=EOF && !trovato){
		fgets(buffer, 80, fdfat);
		char* str=strtok(buffer, SEPARATOR);
		str=strtok(NULL, SEPARATOR);
		if(!str){
			next_inode=i;
			trovato=1;
		}
		i++;
	}
	if(!trovato){
		next_inode=INODE_LIMIT;
	}
	printf("Inode calcolato: %d\n", next_inode);
	ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");
}

void insertInFatFile(char* row, int inode){
	FILE* fdfat=fopen(FAT_FILE_NAME, "r+");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	FILE* fd=fopen(AUX, "w+");
	if(fd==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il appoggio\n");
	char buffer[100]={0};
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, fdfat);
		if(i==inode){
			fputs(row, fd);		
		}else{
			fputs(buffer, fd);
		}
	}
	int ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");
	ret=fclose(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file di appoggio\n");
	
	fdfat=fopen(FAT_FILE_NAME, "w");
	if(fdfat==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file FAT.txt\n");
	fd=fopen(AUX, "r");
	if(fd==NULL) handle_error("Errore: impossibile aprire in letture e in scrittura il file di appoggio\n");
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, fd);
		fputs(buffer, fdfat);
	}
	ret=fclose(fdfat);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file FAT.txt\n");
	ret=fclose(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file di appoggio\n");
	ret=remove(AUX);
	if(ret==-1) handle_error("Errore: impossibile rimuovere il file di appoggio\n");
}

void loadFAT(){
	FILE* FAT=fopen(FAT_FILE_NAME, "r");
	if(FAT==NULL) handle_error("Errore: impossibile aprire FAT.txt in lettura\n");
	char buffer[100]={0};
	char* token;
	for(int i=0; i<MAX_INODE; i++){
		fgets(buffer, 100, FAT);
		token=strtok(buffer, SEPARATOR);
		token=strtok(NULL, SEPARATOR);
		if(token!=NULL){			
			char* nome=token;
			printf("%s\n", nome);
			char* path=strtok(NULL, SEPARATOR);
			char* tipo=strtok(NULL, SEPARATOR);
			char* creatore=strtok(NULL, SEPARATOR);
			int size=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			int padre=strtol(strtok(NULL, SEPARATOR), NULL, 10);
			array_fat[i]=(struct fat*)calloc(1, sizeof(struct fat));
			array_fat[i]->inode=i;
			//array_fat[i]->name=(char*)malloc(sizeof(char)*strlen(nome));
			strcpy(array_fat[i]->name, nome);
			//array_fat[i]->path=(char*)malloc(sizeof(char)*strlen(path));
			strcpy(array_fat[i]->path, path);
			//array_fat[i]->type=(char*)malloc(sizeof(char)*strlen(tipo)); 
			strcpy(array_fat[i]->type, tipo);
			//array_fat[i]->type=tipo;
			array_fat[i]->size=size;//DA RIVEDERE
			//array_fat[i]->creator=(char*)malloc(sizeof(char)*strlen(creatore));
			strcpy(array_fat[i]->creator, creatore);
			array_fat[i]->inode_padre=padre;
			printf("Inode %d, size %d, creator %s, nome %s, percorso %s, tipo %s\n", array_fat[i]->inode, array_fat[i]->size, array_fat[i]->creator, array_fat[i]->name, array_fat[i]->path, array_fat[i]->type);
		}else{
			array_fat[i]=NULL;
		}
	}
	int ret=fclose(FAT);
	if(ret==-1) handle_error("Errore: impossibile chiudere FAT.txt\n");
}

void sharing_father(){
	//CONDIVISIONE DI UN ELEMENTO SINGOLO
	/*int size=sizeof(struct fat)+sizeof(char)*256;
	int fd=shm_open(SHMEM_FOR_INFO, O_CREAT|O_RDWR, 0666);
	if(fd==-1) handle_error("Errore: impossibile creare la zona di memoria condivisa\n");
	int ret=ftruncate(fd, size);
	if(ret==-1) handle_error("Errore: impossibile impostare una dimensione della zona di memoria condivisa\n");
	struct fat* shared=(struct fat*)mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0);
	if(shared==MAP_FAILED) handle_error("Errore: impossibile mappa la zona di memoria condivisa\n");
	printf("Elemento caricato\n");
	memcpy(shared, array_fat[1], size);
	strcpy(shared->name, array_fat[1]->name);
	strcpy(shared->path, array_fat[1]->path);
	strcpy(shared->type, array_fat[1]->type);
	strcpy(shared->creator, array_fat[1]->creator);
	printf("Inode padre %d\n", shared->inode);
	printf("Nome padre %s\n", shared->name);
	printf("Path padre %s\n", shared->path);
	printf("Type padre %s\n", shared->type);
	printf("Creator padre %s\n", shared->creator);
	ret=close(fd);
	if(ret==-1) handle_error("Errore: impossibile chiudere il file descriptor della memoria condivisa");	*/
	
	
	//CONDIVISIONE DELL'ARRAY DI ELEMENTI
	int size=sizeof(struct fat)*MAX_INODE;
	int id=shmget(12345, size, IPC_CREAT | 0666);
	if(id==-1) handle_error("Error shmid\n");
	struct fat* shmem;
	shmem=(struct fat*)shmat(id, 0, 0);
	for(int i=0; i<MAX_INODE; i++){
		//shmem[i]=(struct fat*)malloc(sizeof(struct fat));
		if(array_fat[i]!=NULL){
			shmem[i].inode=array_fat[i]->inode;
			strcpy(shmem[i].name, array_fat[i]->name);
			strcpy(shmem[i].path, array_fat[i]->path);
			strcpy(shmem[i].creator, array_fat[i]->creator);
			strcpy(shmem[i].type, array_fat[i]->type);
			shmem[i].size=array_fat[i]->size;
			shmem[i].inode_padre=array_fat[i]->inode_padre;
		}else{
			shmem[i].inode=INODE_LIMIT;
			strcpy(shmem[i].name, "");
			strcpy(shmem[i].path, "");
			strcpy(shmem[i].creator, "");
			strcpy(shmem[i].type, "");
			shmem[i].size=0;
			shmem[i].inode_padre=0;
		}
	}
	/*printf("Shmem:\n");
	for(int i=0; i<MAX_INODE; i++){
			printf("Inode %d, size %d, creator %s, nome %s, percorso %s, tipo %s, inode_padre %d\n", shmem[i].inode, shmem[i].size, shmem[i].creator,
			shmem[i].name, shmem[i].path, shmem[i].type, shmem[i].inode_padre);
	}*/
	shmdt((void *)shmem);
	printf("Fine shmem\n");
}

void stampaArray(){
	printf("Stampa array:\n");
	for(int i=0; i<MAX_INODE; i++){
		if(array_fat[i]!=NULL){
			printf("Inode %d, size %d, creator %s, nome %s,\npercorso %s, tipo %s, inode padre %d\n\n", array_fat[i]->inode, array_fat[i]->size, array_fat[i]->creator, array_fat[i]->name, array_fat[i]->path, array_fat[i]->type, array_fat[i]->inode_padre);
		}else{
			//printf("Inode %d, NULL\n", i);
		}
	}
}

int searchElement(char* name, char* path){
	printf("Ricerca dell'elemento...\n");
	int trovato=0;
	int inode;
	printf("Nome: %s\nPath: %s\n", name, path);
	for(int i=0; i<MAX_INODE && !trovato; i++){
		if(array_fat[i]!=NULL){
			if(strcmp(array_fat[i]->name, name)==0 && strcmp(array_fat[i]->path, path)==0){
				printf("Elemento trovato\n");
				inode=i;
				trovato=1;
			}else{
				printf("%d: Elemento con nome o percorso diverso\n", i);
			}
		}else{
			printf("%d: Elemento assente\n", i);
		}
	}
	if(trovato){
		printf("Elemento trovato\n");
		return inode;
	}else{
		printf("Elemento non trovato\n");
		return 0;
	}
}

void sendResult(char* res){
	printf("Preparazione invio risultato all'utente\n");
	int fdfifo=open(FIFO_FOR_RES, O_WRONLY);
	printf("fifo aperta\n");
	if(fdfifo==-1){printf("errno: %d\n", errno); handle_error("Errore: impossibile aprire la fifo per comunicare con gli utenti\n");}
	int send_bytes=0;
	int size_elem=strlen(res);
	int ret=0;
	printf("Inviando '%s' all'utente...\n", res);
	while(send_bytes<size_elem){
		ret=write(fdfifo, res+send_bytes, size_elem-send_bytes);
		printf("%d\n", ret);
		if(ret==-1){
			if(errno==EINTR){
				continue;
			}else{
				handle_error("Errore: impossibile mandare il messaggio all'utente\n");
			}
		}else{
			send_bytes+=ret;
		}
	}
	printf("%s\n", res);
	ret=close(fdfifo);
	if(ret==-1) handle_error("Errore: impossibile chiudere il canale di comunicazione\n");
}
