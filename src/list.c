#include "list.h"
#include "stdlib.h"

List* new_list() {
  List* l = malloc(sizeof(List));
  l->head = NULL;
  l->size = 0;
  return l;
}

void list_init(List* l) {
  l->head = NULL;
  l->size = 0;
}

void list_push(List* l, void* data) {
  list_node* n = malloc(sizeof(*n));
  n->data = data;
  n->next = l->head;
  l->head = n;
  l->size++;
}

void* list_top(List* l) {
  return l->head->data;
}

void* list_pop(List* l) {
  if (l->size == 0) {
    return NULL;
  }
  list_node* n = l->head;
  l->head = n->next;
  l->size--;
  void* data = n->data;
  free(n);
  return data;
}

void* list_get(List* l, int index) {
  if (index >= l->size) {
    return NULL;
  }
  list_node* n = l->head;
  for (int i = 0; i < index; i++) {
    n = n->next;
  }
  return n->data;
}

void list_set(List* l, int index, void* data) {
  if (index >= l->size) {
    return;
  }
  list_node* n = l->head;
  for (int i = 0; i < index; i++) {
    n = n->next;
  }
  n->data = data;
}

void list_free(List* l) {
  while (l->size > 0) {
    list_pop(l);
  }
}

void list_map(List *l, void (*f)(void *)) {
  list_node *n = l->head;
  while (n != NULL) {
    f(n->data);
    n = n->next;
  }
}
