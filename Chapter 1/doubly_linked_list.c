#include "doubly_linked_list.h"

#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *value) {
  size_t length = strlen(value) + 1;
  char *copy = malloc(length);
  if (copy != NULL) memcpy(copy, value, length);
  return copy;
}

static ListNode *new_node(const char *value) {
  ListNode *node = malloc(sizeof(*node));
  if (node == NULL) return NULL;

  node->value = copy_string(value);
  if (node->value == NULL) {
    free(node);
    return NULL;
  }

  node->previous = NULL;
  node->next = NULL;
  return node;
}

void list_init(StringList *list) {
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

bool list_push_front(StringList *list, const char *value) {
  ListNode *node = new_node(value);
  if (node == NULL) return false;

  node->next = list->head;
  if (list->head == NULL) {
    list->tail = node;
  } else {
    list->head->previous = node;
  }
  list->head = node;
  list->size++;
  return true;
}

bool list_push_back(StringList *list, const char *value) {
  ListNode *node = new_node(value);
  if (node == NULL) return false;

  node->previous = list->tail;
  if (list->tail == NULL) {
    list->head = node;
  } else {
    list->tail->next = node;
  }
  list->tail = node;
  list->size++;
  return true;
}

ListNode *list_find(const StringList *list, const char *value) {
  for (ListNode *node = list->head; node != NULL; node = node->next) {
    if (strcmp(node->value, value) == 0) return node;
  }
  return NULL;
}

bool list_delete(StringList *list, const char *value) {
  ListNode *node = list_find(list, value);
  if (node == NULL) return false;

  if (node->previous == NULL) {
    list->head = node->next;
  } else {
    node->previous->next = node->next;
  }

  if (node->next == NULL) {
    list->tail = node->previous;
  } else {
    node->next->previous = node->previous;
  }

  free(node->value);
  free(node);
  list->size--;
  return true;
}

void list_clear(StringList *list) {
  ListNode *node = list->head;
  while (node != NULL) {
    ListNode *next = node->next;
    free(node->value);
    free(node);
    node = next;
  }
  list_init(list);
}