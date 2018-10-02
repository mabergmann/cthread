#include<cdata.h>

struct list_node_s{
  TCB_t* value;
  struct list_node_s* next;
};
typedef struct list_node_s list_node;

typedef struct join {
  TCB_t* thread;
  int tid;
} join_t;

struct list_node_join_s{
  join_t* join;
  struct list_node_join_s* next;
};
typedef struct list_node_join_s list_node_join;

void append(list_node** list, void* new_value);

TCB_t* pop_from_list(list_node** list, int tid);

TCB_t* pop_from_queue(list_node** list);

TCB_t* remove_join_from_list(list_node_join** list, int tid);
