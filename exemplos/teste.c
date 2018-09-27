#include "../include/cthread.h"
#include <stdio.h>

void* funcao0(void *arg){
	printf("\n\t Sou a thread 0 executando.\n");

	return;
}

void* funcao1(void *arg){
	printf("\n\t Sou a thread 1 executando.\n");

	return;
}

int main(int argc, char *argv[]){

	int id0, id1, i;

	printf("\n\tSou a main executando.\n");
	id0 = ccreate(funcao0, (void *)&i, 0);
	id1 = ccreate(funcao1, (void *)&i, 0);

	printf("\n\tMain criou as threads.\n");

	return 0;
}
