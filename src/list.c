#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void append(list_node** list, void* new_value){
  list_node* current;
  list_node* new_node;
  current = *list;
  new_node = malloc(sizeof(list_node));
  new_node->value = (void*)new_value;
  new_node->next = NULL;
  if(!current){
    *list = new_node;
  }
  else {
    while(current->next){
      current = current->next;
    }
    current->next = new_node;
  }
}

TCB_t* pop_from_list(list_node** list, int tid){
  list_node* current;
  list_node* previous=NULL;
  current = *list;
  while(current){
    if(current->value->tid == tid){ // Found the node
      if(!previous){
	*list = current->next;
	return current->value;
      }else{
	previous->next = current->next;
	return current->value;
      }
    }
    previous = current;
    current = current->next;
  }
  return NULL;
}

TCB_t* pop_from_queue(list_node** list){
  TCB_t* thread;
  thread = (*list)->value;
  *list = (*list)->next;
  return thread;
}
