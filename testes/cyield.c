#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

int i;

void* func2(void *arg){

	printf("\n\tSou a thread 2 vou fazer cyield.\n");

	cyield();

	printf("\n\tSou a thread 2 vou fazer o segundo cyiled.\n");

    cyield();

	printf("\n\tSou a thread 2 voltando a execucao e terminando.\n");

	return NULL;
}

void* func1(void *arg){

	printf("\n\tSou a thread 1 vou criar a thread 2.\n");

    ccreate(func2, (void *)&i, 1);

	printf("\n\tSou a thread 1 vou fazer cyield.\n");

	cyield();

    printf("\n\tSou a thread 1 vou fazer o segundo cyiled.\n");

    cyield();

	printf("\n\tSou a thread 1 voltando a execucao e terminando.\n");

	return NULL;
}

int main(int argc, char *argv[]){

	printf("\n\tSou a main executando.\n");

    ccreate(func1, (void *)&i, 1);

	printf("\n\tSou a main voltando e terminando.\n");

	return 0;
}

