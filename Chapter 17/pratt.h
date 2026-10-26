#ifndef CHAPTER17_PRATT_H
#define CHAPTER17_PRATT_H

#include <stdbool.h>
#include <stddef.h>

bool parseExpression(const char* source, char* output, size_t capacity);
const char* challengeTrace(void);

typedef struct {
  bool prefix;
  bool infixOrPostfix;
} TokenRoles;

TokenRoles rolesFor(char token);

#endif

