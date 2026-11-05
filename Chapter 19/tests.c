#include "strings.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  FlexString* hello = flexCopy("hello", 5);
  FlexString* space = flexCopy(" world", 6);
  FlexString* joined = flexConcat(hello, space);
  assert(strcmp(joined->chars, "hello world") == 0);
  assert((char*)hello->chars == (char*)hello + offsetof(FlexString, chars));
  flexFree(hello);
  flexFree(space);
  flexFree(joined);

  static const char literal[] = "borrowed";
  ManagedString* borrowed = managedBorrow(literal, 8);
  ManagedString* owned = managedCopy(" owned", 6);
  ManagedString* managed = managedConcat(borrowed, owned);
  assert(!borrowed->ownsChars && owned->ownsChars && managed->ownsChars);
  assert(strncmp(managed->chars, "borrowed owned", managed->length) == 0);

  TextValue stringValue = {TEXT_STRING, {.string = borrowed}};
  TextValue numberValue = {TEXT_NUMBER, {.number = 42}};
  ManagedString* mixed = concatValues(stringValue, numberValue);
  assert(strcmp(mixed->chars, "borrowed42") == 0);
  TextValue a = {TEXT_NUMBER, {.number = 1}};
  TextValue b = {TEXT_NUMBER, {.number = 2}};
  assert(concatValues(a, b) == NULL);

  managedFree(borrowed);
  managedFree(owned);
  managedFree(managed);
  managedFree(mixed);
  puts("chapter19: all tests passed");
  return 0;
}

