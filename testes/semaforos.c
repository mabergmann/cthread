#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

int i;
csem_t semaforo;

void* func2(void *arg)
{
    printf("\n\tSou a thread 2 vou fazer cwait.\n");

    cwait(&semaforo);

    printf("\n\tSou a thread 2 usando o semaforo.\n");

    cyield();

    printf("\n\tSou a thread 2 liberando semaforo e terminando.\n");

    csignal(&semaforo);

    return NULL;
}

void* func1(void *arg)
{
    printf("\n\tSou a thread 1 vou criar a thread 2.\n");

    ccreate(func2, (void *)&i,2);

    printf("\n\tSou a thread 1 vou fazer cwait e cyield.\n");

    cwait(&semaforo);

    cyield();

    printf("\n\tSou a thread 1 voltando a execucao, fazendo csignal e cyield.\n");

    csignal(&semaforo);

    cyield();

    printf("\n\tSou a thread 1 voltando e fazendo cwait\n");

    cwait(&semaforo);

    printf("\n\tSou a thread 1 usando o semaforo pela segunda vez, e liberando.\n\n");

    csignal(&semaforo);

    return NULL;
}

int main(int argc, char *argv[])
{
    int id1;
    char name[85];

    printf("Eu sou a main iniciando o semaforo.\n");

    csem_init(&semaforo, 1);

    printf("Semaforo inicializado.\n");

    printf("Eu sou a main criando de ID1\n");

    id1 = ccreate(func1, (void *)&i, 2);

    cjoin(id1);

    printf("Eu sou a main voltando e vou terminar o programa.\n");

    cidentify(name,85);

    printf("%s", name);

    return 0;
}

