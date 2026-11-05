#include "strings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned hashBytes(const char* chars, size_t length) {
  unsigned hash = 2166136261u;
  for (size_t i = 0; i < length; i++) {
    hash ^= (unsigned char)chars[i];
    hash *= 16777619u;
  }
  return hash;
}

FlexString* flexCopy(const char* chars, size_t length) {
  FlexString* string = malloc(sizeof(FlexString) + length + 1);
  if (string == NULL) return NULL;
  string->length = length;
  memcpy(string->chars, chars, length);
  string->chars[length] = '\0';
  string->hash = hashBytes(string->chars, length);
  return string;
}

FlexString* flexConcat(const FlexString* left, const FlexString* right) {
  size_t length = left->length + right->length;
  FlexString* result = malloc(sizeof(FlexString) + length + 1);
  if (result == NULL) return NULL;
  result->length = length;
  memcpy(result->chars, left->chars, left->length);
  memcpy(result->chars + left->length, right->chars, right->length);
  result->chars[length] = '\0';
  result->hash = hashBytes(result->chars, length);
  return result;
}

void flexFree(FlexString* string) { free(string); }

static ManagedString* makeManaged(const char* chars, size_t length, bool owns) {
  ManagedString* string = malloc(sizeof(ManagedString));
  if (string == NULL) return NULL;
  string->length = length;
  string->hash = hashBytes(chars, length);
  string->ownsChars = owns;
  string->chars = chars;
  return string;
}

ManagedString* managedBorrow(const char* chars, size_t length) {
  return makeManaged(chars, length, false);
}

ManagedString* managedCopy(const char* chars, size_t length) {
  char* copy = malloc(length + 1);
  if (copy == NULL) return NULL;
  memcpy(copy, chars, length);
  copy[length] = '\0';
  ManagedString* string = makeManaged(copy, length, true);
  if (string == NULL) free(copy);
  return string;
}

ManagedString* managedConcat(const ManagedString* left, const ManagedString* right) {
  size_t length = left->length + right->length;
  char* chars = malloc(length + 1);
  if (chars == NULL) return NULL;
  memcpy(chars, left->chars, left->length);
  memcpy(chars + left->length, right->chars, right->length);
  chars[length] = '\0';
  ManagedString* result = makeManaged(chars, length, true);
  if (result == NULL) free(chars);
  return result;
}

void managedFree(ManagedString* string) {
  if (string == NULL) return;
  if (string->ownsChars) free((void*)string->chars);
  free(string);
}

static ManagedString* valueText(TextValue value) {
  char buffer[64];
  switch (value.type) {
    case TEXT_STRING:
      return managedCopy(value.as.string->chars, value.as.string->length);
    case TEXT_NUMBER:
      snprintf(buffer, sizeof(buffer), "%g", value.as.number);
      break;
    case TEXT_BOOL:
      snprintf(buffer, sizeof(buffer), "%s", value.as.boolean ? "true" : "false");
      break;
    case TEXT_NIL:
      snprintf(buffer, sizeof(buffer), "nil");
      break;
  }
  return managedCopy(buffer, strlen(buffer));
}

ManagedString* concatValues(TextValue left, TextValue right) {
  if (left.type != TEXT_STRING && right.type != TEXT_STRING) return NULL;
  ManagedString* leftText = valueText(left);
  ManagedString* rightText = valueText(right);
  if (leftText == NULL || rightText == NULL) {
    managedFree(leftText);
    managedFree(rightText);
    return NULL;
  }
  ManagedString* result = managedConcat(leftText, rightText);
  managedFree(leftText);
  managedFree(rightText);
  return result;
}

