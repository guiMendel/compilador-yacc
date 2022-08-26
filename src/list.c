#include "list.h"
#include "stdlib.h"

void list_init(list* l) {
  l->head = NULL;
  l->size = 0;
}

void list_push(list* l, void* data) {
  list_node* n = malloc(sizeof(*n));
  n->data = data;
  n->next = l->head;
  l->head = n;
  l->size++;
}

void* list_top(list* l) {
  return l->head->data;
}

void* list_pop(list* l) {
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

void* list_get(list* l, int index) {
  if (index >= l->size) {
    return NULL;
  }
  list_node* n = l->head;
  for (int i = 0; i < index; i++) {
    n = n->next;
  }
  return n->data;
}

void list_set(list* l, int index, void* data) {
  if (index >= l->size) {
    return;
  }
  list_node* n = l->head;
  for (int i = 0; i < index; i++) {
    n = n->next;
  }
  n->data = data;
}

void list_free(list* l) {
  while (l->size > 0) {
    list_pop(l);
  }
}

void list_map(list *l, void (*f)(void *)) {
  list_node *n = l->head;
  while (n != NULL) {
    f(n->data);
    n = n->next;
  }
}
