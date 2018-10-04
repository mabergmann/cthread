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
list_node* terminated;
list_node_join* join_list;
TCB_t *executing;

int next_tid = 0;
void scheduler();
void add_thread_to_ready(TCB_t *new_thread);
bool process_queue_is_empty();
TCB_t* select_next_thread_and_unqueue();
TCB_t* pop_thread_by_tid(int tid);
void create_scheduler_context();
void terminate_thread(TCB_t* thread);
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
    int i;
    for(i=0; i<NUM_PRIO; i++)
    {
        if (queues[i]!=NULL)
            return pop_from_queue(&queues[i]);
    }
    return NULL;
}

void create_scheduler_context()
{
    scheduler_context = malloc(sizeof(ucontext_t));
    getcontext(scheduler_context);
    scheduler_context->uc_stack.ss_sp = malloc(SIGSTKSZ);
    scheduler_context->uc_stack.ss_size = (SIGSTKSZ);
    makecontext(scheduler_context, (void(*)(void)) scheduler, 0);
}

void terminate_thread(TCB_t* thread)
{
    TCB_t* joined_thread;
    thread->state = PROCST_TERMINO;
    append(&terminated, executing);
    joined_thread = remove_join_from_list(&join_list, thread->tid);
    if(joined_thread != NULL)
    {
        joined_thread->state = PROCST_APTO;
        add_thread_to_ready(joined_thread);
    }
}

void scheduler()
{
    TCB_t* next_thread;
    while(!process_queue_is_empty())
    {
        next_thread = select_next_thread_and_unqueue();
        executing = next_thread;
        executing->state = PROCST_EXEC;
        swapcontext(scheduler_context,&(executing->context));
        if(executing->state == PROCST_EXEC)  // The thread finished
        {
            terminate_thread(executing);
        }
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
    terminated = NULL;
    join_list = NULL;
}

void initialize()
{
    initialized = TRUE;
    create_scheduler_context();
    intialize_queues();
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
    executing->state = PROCST_APTO;
    append(&queues[executing->prio], (void*)executing);
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
    if(executing->prio > prio)   // Should be preempted
    {
        preempt();
    }

    return 0;
}

int cyield(void)
{
    TCB_t* thread_ced;

    if(!initialized)
        initialize();

    thread_ced = malloc(sizeof(TCB_t));
    thread_ced = executing;
    executing->state = PROCST_APTO;

    add_thread_to_ready(thread_ced);

    swapcontext(&executing->context, scheduler_context);

    return 0;
}

int cjoin(int tid)
{
    join_t* join;
    if(!initialized)
        initialize();

    if(is_tid_in_list(terminated, tid))
        return 0;

    join = malloc(sizeof(join_t));
    join->thread = executing;
    join->tid = tid;

    append_join(&join_list, join);
    executing->state = PROCST_BLOQ;
    swapcontext(&executing->context, scheduler_context);
    return 0;
}

int csem_init(csem_t *sem, int count)
{
    if(!initialized)
    {
        initialize();
    }
    sem->count = count;
    sem->fila = malloc(sizeof(FILA2));
    CreateFila2(sem->fila);

    return 0;
}

int cwait(csem_t *sem)
{
    TCB_t* waiting_thread;

    if(!initialized)
        initialize();

    if(sem->count < 1)
    {
        sem->count = sem->count - 1;
        waiting_thread = malloc(sizeof(TCB_t));
        waiting_thread = executing;
        executing->state = PROCST_BLOQ;
        AppendFila2((PFILA2)(sem->fila), waiting_thread);
        swapcontext(&executing->context, scheduler_context);
    }
    else
    {
        sem->count = sem->count - 1;
    }

    return 0;
}


void* getPriorityThread(PFILA2 fila)
{

    TCB_t *thread_ret, *thread_aux;

    FirstFila2(fila);

    thread_ret = GetAtIteratorFila2(fila);

    while(NextFila2(fila)==0)
    {
        thread_aux = GetAtIteratorFila2(fila);
        if(thread_ret->prio > thread_aux->prio)
        {
            thread_ret = thread_aux;
        }
    }

    FirstFila2(fila);

    while(thread_ret != GetAtIteratorFila2(fila))
    {
        NextFila2(fila);
    }

    DeleteAtIteratorFila2(fila);

    return thread_ret;
}

int csignal(csem_t *sem)
{
    TCB_t* thread_apt;

    if(!initialized)
        initialize();

    if(FirstFila2(sem->fila)==0)
    {
        thread_apt = getPriorityThread(sem->fila);
        add_thread_to_ready(thread_apt);
        //FirstFila2(sem->fila);
        //DeleteAtIteratorFila2(sem->fila);
        sem->count = sem->count + 1;

        if(executing->prio > thread_apt->prio)
        {
            preempt();
        }
    }
    else
    {
        sem->count = sem->count + 1;
    }

    return 0;
}

//COMPLETAR O NOME DA ROSANA E A MATRICULA.
int cidentify (char *name, int size)
{
    //84
    strncpy (name, " Henrique Goetz - 274719\n Matheus Alan Bergmann - 274704\n Rosana Dornelles - 000000\n", size);

    return 0;
}



