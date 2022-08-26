#include "fun_main.h"

#include <stdio.h>

void printInfo(){
	printf("In questo sistema potrai eseguire diverse operazioni su file e directory con i seguenti comandi:\n");
	printf("\tcf -> comando per creare un nuovo file\n");
	printf("\tef -> comando per eliminare un file\n");
	printf("\twf -> comando per scrivere in un file\n");
	printf("\trf -> comando per leggere il contenuto di un file\n");
	printf("\tsf -> comando per spostare il cursore dentro al file\n");
	printf("\tcd -> comando per creare una nuova directory\n");
	printf("\ted -> comando per eliminare una directory\n");
	printf("\tmd -> comando per cambiare directory corrente\n");
	printf("\tld -> comando per stampare gli elementi presenti nella direcoty corrente\n");
	printf("\thelp -> comando per visualizzare i comandi eseguibili nel sistema\n");
	printf("\texit -> comando per chiudere il sistema\n");
}
