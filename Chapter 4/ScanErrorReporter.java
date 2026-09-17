package com.craftinginterpreters.lox;

@FunctionalInterface
interface ScanErrorReporter {
  void report(int line, String message);
}

