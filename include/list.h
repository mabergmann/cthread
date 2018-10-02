#include<cdata.h>

struct list_node_s{
  TCB_t* value;
  struct list_node_s* next;
};
typedef struct list_node_s list_node;

void append(list_node** list, void* new_value);

TCB_t* pop_from_list(list_node** list, int tid);

TCB_t* pop_from_queue(list_node** list);
