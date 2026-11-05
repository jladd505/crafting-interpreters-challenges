#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNT 12000

static double seconds(clock_t start) { return (double)(clock() - start) / CLOCKS_PER_SEC; }

static void numericWorkload(bool collisions, bool churn) {
  Table table;
  initTable(&table, collisions);
  clock_t start = clock();
  double checksum = 0;
  for (int i = 0; i < COUNT; i++) tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i * 2));
  if (churn) {
    for (int i = 0; i < COUNT; i += 2) tableDelete(&table, NUMBER_VAL(i));
    for (int i = 0; i < COUNT; i += 2) tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i * 3));
  }
  for (int pass = 0; pass < 8; pass++) {
    for (int i = 0; i < COUNT; i++) {
      Value value;
      if (tableGet(&table, NUMBER_VAL(i), &value)) checksum += value.as.number;
    }
  }
  printf("%-24s %8.4f s  checksum %.0f\n",
         collisions ? "forced collisions" : churn ? "deletion churn" : "sequential numbers",
         seconds(start), checksum);
  freeTable(&table);
}

static void updateWorkload(void) {
  Table table;
  initTable(&table, false);
  clock_t start = clock();
  for (int i = 0; i < 100000; i++) tableSet(&table, STRING_VAL("counter"), NUMBER_VAL(i));
  Value value;
  tableGet(&table, STRING_VAL("counter"), &value);
  printf("%-24s %8.4f s  checksum %.0f\n", "repeated update", seconds(start), value.as.number);
  freeTable(&table);
}

int main(void) {
  puts("Chapter 20 hash-table benchmark");
  numericWorkload(false, false);
  numericWorkload(false, true);
  numericWorkload(true, false);
  updateWorkload();
  return 0;
}

