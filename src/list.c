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

int is_tid_in_list(list_node* list, int tid){
  while(list){
    if(list->value->tid == tid)
      return 1;
    list = list->next;
  }
  return 0;
}

void append_join(list_node_join** list, join_t* new_value){
  list_node_join* current;
  list_node_join* new_node;
  current = *list;
  new_node = malloc(sizeof(list_node_join));
  new_node->join = new_value;
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

TCB_t* remove_join_from_list(list_node_join** list, int tid){
  list_node_join* current;
  list_node_join* previous=NULL;
  TCB_t* thread;
  current = *list;
  while(current){
    if(current->join->tid == tid){ // Found the node
      if(!previous){
	*list = current->next;
	thread = current->join->thread;
	return thread;
      }else{
	previous->next = current->next;
	thread = current->join->thread;
	return thread;
      }
    }
    previous = current;
    current = current->next;
  }
  return NULL;
}
