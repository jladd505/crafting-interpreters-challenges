#ifndef CHAPTER19_STRINGS_H
#define CHAPTER19_STRINGS_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  size_t length;
  unsigned hash;
  char chars[];
} FlexString;

FlexString* flexCopy(const char* chars, size_t length);
FlexString* flexConcat(const FlexString* left, const FlexString* right);
void flexFree(FlexString* string);

typedef struct {
  size_t length;
  unsigned hash;
  bool ownsChars;
  const char* chars;
} ManagedString;

ManagedString* managedBorrow(const char* chars, size_t length);
ManagedString* managedCopy(const char* chars, size_t length);
ManagedString* managedConcat(const ManagedString* left, const ManagedString* right);
void managedFree(ManagedString* string);

typedef enum { TEXT_NUMBER, TEXT_BOOL, TEXT_NIL, TEXT_STRING } TextValueType;
typedef struct {
  TextValueType type;
  union {
    double number;
    bool boolean;
    const ManagedString* string;
  } as;
} TextValue;

ManagedString* concatValues(TextValue left, TextValue right);

#endif

