#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

int i;
int id1;


void* func2(void *arg)
{
    printf("\n\tSou a thread 2 vou fazer cjoin(1).\n");

    if(cjoin(id1) ==-1)
    printf("\nDeu ERRO!\n\n");

    printf("\n\tSou a thread 2 terminando.\n");

    return NULL;
}

void* func1(void *arg)
{
    int id2;

    printf("\n\tSou a thread 1 vou criar a thread 2.\n");

    id2 = ccreate(func2, (void *)&i,2);

    printf("\n\tSou a thread 1 vou fazer cyield.\n");

    cyield();

    printf("\n\tSou a thread 1 terminando.\n\n");


    return NULL;
}

int main(int argc, char *argv[])
{
    char name[85];

    printf("Eu sou a main criando de ID1 e dando cjoin(id1)\n");

    id1 = ccreate(func1, (void *)&i, 2);

    cjoin(id1);

    printf("Eu sou a main voltando e vou terminar o programa.\n");

    cidentify(name,85);

    printf("%s", name);

    return 0;
}
