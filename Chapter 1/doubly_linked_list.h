#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ListNode {
  char *value;
  struct ListNode *previous;
  struct ListNode *next;
} ListNode;

typedef struct {
  ListNode *head;
  ListNode *tail;
  size_t size;
} StringList;

void list_init(StringList *list);
bool list_push_front(StringList *list, const char *value);
bool list_push_back(StringList *list, const char *value);
ListNode *list_find(const StringList *list, const char *value);
bool list_delete(StringList *list, const char *value);
void list_clear(StringList *list);

#endif