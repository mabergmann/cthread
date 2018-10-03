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

    ccreate(func2, (void *)&i,1);

    printf("\n\tSou a thread 1 vou fazer cwait e cyield.\n");

    cwait(&semaforo);

    cyield();

    printf("\n\tSou a thread 1 voltando a execucao, fazendo csignal e cyield.\n");

    csignal(&semaforo);

    cyield();

    cwait(&semaforo);

    printf("\n\tSou a thread 1 usando o semaforo pela segunda vez, e liberando");

    csignal(&semaforo);

    return NULL;
}

int main(int argc, char *argv[])
{
    printf("Eu sou a main iniciando o semaforo.\n");

    csem_init(&semaforo, 1);

    printf("Semaforo inicializado.\n");

    printf("Eu sou a main criando de ID1\n");

    ccreate(func1, (void *)&i, 1);

    cyield();

    printf("Eu sou a main voltando para terminar o programa\n");

    return 0;
}

