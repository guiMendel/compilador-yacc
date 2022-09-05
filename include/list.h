#ifndef LIST_H
#define LIST_H

typedef struct list_node {
  void* data;
  struct list_node* next;
} list_node;

typedef struct {
  list_node* head;
  list_node* tail;
  int size;
} List;

void list_init(List* l);
void list_push(List* l, void* data);
void list_append(List* l, void* data);
void* list_top(List* l);
void* list_pop(List* l);
void* list_get(List* l, int index);
void list_set(List* l, int index, void* data);
void list_free(List* l);
void list_map(List* l, void (*f)(void*));

List* new_list();

#endif // LIST_H
