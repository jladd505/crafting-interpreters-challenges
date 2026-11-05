#include "table.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  Table table;
  initTable(&table, false);
  assert(tableSet(&table, NIL_VAL, STRING_VAL("nil-key")));
  assert(tableSet(&table, BOOL_VAL(true), NUMBER_VAL(1)));
  assert(tableSet(&table, NUMBER_VAL(42), STRING_VAL("number")));
  assert(tableSet(&table, STRING_VAL("name"), BOOL_VAL(false)));
  assert(!tableSet(&table, STRING_VAL("name"), BOOL_VAL(true)));

  Value value;
  assert(tableGet(&table, NIL_VAL, &value) && value.type == VAL_STRING);
  assert(tableGet(&table, NUMBER_VAL(42), &value) && value.type == VAL_STRING);
  assert(tableGet(&table, STRING_VAL("name"), &value) && value.as.boolean);
  assert(tableSet(&table, NUMBER_VAL(-0.0), NUMBER_VAL(7)));
  assert(tableGet(&table, NUMBER_VAL(0.0), &value) && value.as.number == 7);
  assert(tableDelete(&table, BOOL_VAL(true)));
  assert(!tableGet(&table, BOOL_VAL(true), &value));
  assert(tableSet(&table, BOOL_VAL(false), NUMBER_VAL(2)));
  freeTable(&table);
  puts("chapter20: all tests passed");
  return 0;
}

