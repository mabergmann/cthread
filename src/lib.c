
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"

typedef int bool;

#define TRUE 1
#define FALSE 0

#define NUM_PRIO 3

bool initialized = FALSE;
ucontext_t* scheduler_context;

FILA2 queues[NUM_PRIO];
TCB_t *executing;

int next_tid = 0;

bool process_queue_is_empty(){
  int i,n;
  for(i=0; i<NUM_PRIO; i++){
    n = FirstFila2(&queues[i]);
    if(n==0){ // If it was able to set the iterator, then the queue is not empty
      return FALSE;
    }
  }
  return TRUE;
}

TCB_t* select_next_thread_and_unqueue(){
  int i,n;
  TCB_t *thread;
  for(i=0; i<NUM_PRIO; i++){
    n = FirstFila2(&queues[i]);
    if(n==0){ // If it was able to set the iterator, then the queue is not empty
      thread = (TCB_t*)GetAtIteratorFila2(&queues[i]);
      DeleteAtIteratorFila2(&queues[i]);
      return thread;
    }
  }
}

void scheduler(){
  TCB_t* next_thread;
  while(!process_queue_is_empty()){
    // TODO: implement scheduler
    next_thread = select_next_thread_and_unqueue();
    executing = next_thread;
    executing->state = PROCST_EXEC;
    executing->context.uc_link = malloc(sizeof(ucontext_t));
    getcontext(executing->context.uc_link);
    printf("Scheduling %d\n", executing->tid);
    swapcontext(scheduler_context, &(executing->context));
    printf("Voltou\n\n");
  }
}

void create_scheduler_context(){
  scheduler_context = malloc(sizeof(ucontext_t));
  getcontext(scheduler_context);
  scheduler_context->uc_stack.ss_sp = malloc(SIGSTKSZ);
  scheduler_context->uc_stack.ss_size = (SIGSTKSZ);
  makecontext(scheduler_context, (void(*)(void)) scheduler, 0);
}

TCB_t* create_main_thread(){
  TCB_t* main_tcb;

  main_tcb = malloc(sizeof(TCB_t));

  main_tcb->tid=next_tid;
  next_tid++;
  main_tcb->state=PROCST_EXEC;
  main_tcb->prio=2;
  getcontext(&main_tcb->context);

  return main_tcb;
}

void intialize_queues(){
  int i;
  for(i=0;i<NUM_PRIO;i++){
    CreateFila2(&queues[i]);
  }
}

void initialize(){
  TCB_t main_tcb;

  initialized = TRUE;

  create_scheduler_context();

  void intialize_queues();

  executing = create_main_thread();
}

TCB_t* create_new_thread(void* (*start)(void*), void *arg, int prio){
  TCB_t* new_thread;
  new_thread = malloc(sizeof(TCB_t));

  new_thread->tid = next_tid;
  next_tid++;

  new_thread->state = PROCST_CRIACAO;
  new_thread->prio = prio;
  getcontext(&new_thread->context);
  new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
  new_thread->context.uc_stack.ss_size = (SIGSTKSZ);
  makecontext(&new_thread->context, (void(*)(void)) start, 1, arg);

  return new_thread;
}

void preempt(){
  AppendFila2(&queues[executing->prio], (void*)executing);
  executing->state = PROCST_APTO;
  printf("Going into scheduler context\n");
  swapcontext(&executing->context, scheduler_context);
}

add_thread_to_ready(TCB_t *new_thread){
  AppendFila2(&queues[new_thread->prio], new_thread);
}

int ccreate (void* (*start)(void*), void *arg, int prio) {
  TCB_t* new_thread;

  if(!initialized)
    initialize();

  if(prio > 2 || prio < 0)
    return -1;

  new_thread = create_new_thread(start, arg, prio);

  add_thread_to_ready(new_thread);

  if(executing->prio > prio){ // Should be preempted
    preempt();
  }
  return 0;
}

int csetprio(int tid, int prio) {
  return -1;
}

int cyield(void) {
  return -1;
}

int cjoin(int tid) {
  return -1;
}

int csem_init(csem_t *sem, int count) {
  return -1;
}

int cwait(csem_t *sem) {
  return -1;
}

int csignal(csem_t *sem) {
  return -1;
}

int cidentify (char *name, int size) {
  strncpy (name, "Sergio Cechin - 2017/1 - Teste de compilacao.", size);
  printf("\n\t %s", name)
  return 0;
}


