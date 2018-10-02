#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/list.h"

typedef int bool;

#define TRUE 1
#define FALSE 0

#define NUM_PRIO 3

bool initialized = FALSE;
ucontext_t* scheduler_context;

list_node* queues[NUM_PRIO];
TCB_t *executing;

int next_tid = 0;
void scheduler();

TCB_t* pop_thread_by_tid(int tid)
{
    int i;
    TCB_t* thread;
    for(i=0; i<NUM_PRIO; i++)
    {
        thread = pop_from_list(&queues[i], tid);
        if(thread != NULL)
            return thread;
    }
    return NULL;
}

bool process_queue_is_empty()
{
    int i;
    for(i=0; i<NUM_PRIO; i++)
    {
        if(queues[i] != NULL)
            return FALSE;
    }
    return TRUE;
}

TCB_t* select_next_thread_and_unqueue()
{
    int i,n;
    TCB_t *thread;
    for(i=0; i<NUM_PRIO; i++)
    {
        if (queues[i]!=NULL)
            return pop_from_queue(&queues[i]);
    }
}

void create_scheduler_context()
{
    scheduler_context = malloc(sizeof(ucontext_t));
    getcontext(scheduler_context);
    scheduler_context->uc_stack.ss_sp = malloc(SIGSTKSZ);
    scheduler_context->uc_stack.ss_size = (SIGSTKSZ);
    makecontext(scheduler_context, (void(*)(void)) scheduler, 0);
}

void scheduler()
{
    TCB_t* next_thread;
    while(!process_queue_is_empty())
    {
        next_thread = select_next_thread_and_unqueue();
        executing = next_thread;
        executing->state = PROCST_EXEC;
        create_scheduler_context();
        printf("Scheduling %d\n", executing->tid);
        printf("uc_link = %p\n", executing->context.uc_link);
        swapcontext(scheduler_context,&(executing->context));
        printf("Voltou\n\n");
    }
}

TCB_t* create_main_thread()
{
    TCB_t* main_tcb;

    main_tcb = malloc(sizeof(TCB_t));

    main_tcb->tid=next_tid;
    next_tid++;
    main_tcb->state=PROCST_EXEC;
    main_tcb->prio=2;
    getcontext(&main_tcb->context);

    return main_tcb;
}

void intialize_queues()
{
    int i;
    for(i=0; i<NUM_PRIO; i++)
    {
        queues[i]=NULL;
    }
}

void initialize()
{
    TCB_t main_tcb;

    initialized = TRUE;

    create_scheduler_context();

    void intialize_queues();

    executing = create_main_thread();
}

TCB_t* create_new_thread(void* (*start)(void*), void *arg, int prio)
{
    TCB_t* new_thread;
    new_thread = malloc(sizeof(TCB_t));

    new_thread->tid = next_tid;
    next_tid++;

    new_thread->state = PROCST_CRIACAO;
    new_thread->prio = prio;
    getcontext(&new_thread->context);
    new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    new_thread->context.uc_stack.ss_size = (SIGSTKSZ);
    new_thread->context.uc_link = scheduler_context;
    makecontext(&new_thread->context, (void(*)(void)) start, 1, arg);

    return new_thread;
}

void preempt()
{
    append(&queues[executing->prio], (void*)executing);
    executing->state = PROCST_APTO;
    swapcontext(&executing->context, scheduler_context);
}

void add_thread_to_ready(TCB_t *new_thread)
{
    append(&(queues[new_thread->prio]), new_thread);
}

int ccreate (void* (*start)(void*), void *arg, int prio)
{
    TCB_t* new_thread;

    if(!initialized)
        initialize();

    if(prio > 2 || prio < 0)
        return -1;

    new_thread = create_new_thread(start, arg, prio);
    add_thread_to_ready(new_thread);

    if(executing->prio > prio)  // Should be preempted
    {
        preempt();
    }
    return new_thread->tid;
}

int csetprio(int tid, int prio)
{
    TCB_t* thread;

    if(!initialized)
        initialize();

    if(prio > 2 || prio < 0)
        return -1;


    thread = pop_thread_by_tid(tid);
    if(thread == NULL) return -2;
    add_thread_to_ready(thread);
    if(executing->prio > prio)  // Should be preempted
    {
        preempt();
    }

    return 0;
}

int cyield(void)
{
    if(!initialized)
        initialize();

    executing->state = PROCST_APTO;

    add_thread_to_ready(executing);

    swapcontext(&executing->context, scheduler_context);
    return 0;
}

int cjoin(int tid)
{
    return -1;
}

int csem_init(csem_t *sem, int count)
{
    if(!initialized)
        initialize();

        sem->count = count;

        CreateFila2((sem->fila));

    return 0;
}

int cwait(csem_t *sem)
{
    if(!initialized)
        initialize();

    if(sem->count < 1){
        sem->count = sem->count - 1;
        AppendFila2((sem->fila), executing);
        swapcontext(&executing->context, scheduler_context);
    }else{
        sem->count = sem->count - 1;
    }

    return 0;
}

int csignal(csem_t *sem)
{
    if(!initialized)
        initialize();

    if(FirstFila2(sem->fila)==0){
        add_thread_to_ready(GetAtIteratorFila2((sem->fila)));
        FirstFila2(sem->fila);
        DeleteAtIteratorFila2(sem->fila);
        sem->count = sem->count + 1;
    }
    return 0;
}

//COMPLETAR O NOME DA ROSANA E A MATRICULA.
int cidentify (char *name, int size)
{
    strncpy (name, "\n\tHenrique Goetz - 274719\n\tMatheus Alan Bergmann - 274704\n\tRosana", size);
    printf("%s", name);
    return 0;
}


