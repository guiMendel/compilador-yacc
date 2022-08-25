#ifndef LIST_H
#define LIST_H

typedef struct list_node {
  void* data;
  struct list_node* next;
} list_node;

typedef struct {
  list_node* head;
  int size;
} list;

void list_init(list* l);
void list_push(list* l, void* data);
void* list_pop(list* l);
void* list_get(list* l, int index);
void list_set(list* l, int index, void* data);
void list_free(list* l);
void list_map(list* l, void (*f)(void*));

#endif // LIST_H
