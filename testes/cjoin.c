#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

int i;
csem_t semaforo1, semaforo2;

void* func3(void *arg)
{
    printf("\n\tSou a thread 3 executando e farei cwait para semaforo2.\n");

    cwait(&semaforo2);

    printf("\n\tSou a thread 3 usando semaforo2.\n");

    csignal(&semaforo2);

    printf("\n\tSou a thread 3 fazendo csignal e terminando.\n");

    return NULL;
}


void* func2(void *arg)
{
    int id3;

    printf("\n\tSou a thread 2 vou criar a thread 3.\n");

    id3 = ccreate(func3, (void *)&i,2);

    printf("\n\tSou a thread 2 vou fazer cwait para semaforo2.\n");

    cwait(&semaforo2);

    printf("\n\tSou a thread 2 usando o semaforo2.\n");

    printf("\n\tSou a thread 2 fazendo cyield no trecho critico.\n");

    cyield();

    printf("\n\tSou a thread 2 liberando semaforo2 e terminando.\n");

    csignal(&semaforo2);

    cjoin(id3);

    return NULL;
}

void* func1(void *arg)
{
    int id2;

    printf("\n\tSou a thread 1 vou criar a thread 2.\n");

    id2 = ccreate(func2, (void *)&i,2);

    printf("\n\tSou a thread 1 vou fazer cwait e cyield.\n");

    cwait(&semaforo1);

    printf("\n\tThread 1 usando semaforo1\n");

    printf("\n\tThread 1 vai fazer cjoin para thread 2.\n");
    cjoin(id2);

    csignal(&semaforo1);

    printf("\n\tSou a thread 1 liberando semaforo e terminando.\n\n");


    return NULL;
}

int main(int argc, char *argv[])
{
    int id1;
    char name[85];

    printf("Eu sou a main iniciando o semaforo.\n");

    csem_init(&semaforo1, 1);

    csem_init(&semaforo2, 1);

    printf("Semaforo inicializado.\n");

    printf("Eu sou a main criando de ID1\n");

    id1 = ccreate(func1, (void *)&i, 2);

    cjoin(id1);

    printf("Eu sou a main voltando e vou terminar o programa.\n");

    cidentify(name,85);

    printf("%s", name);

    return 0;
}
