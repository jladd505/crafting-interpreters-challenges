package com.craftinginterpreters.lox;

@FunctionalInterface
interface Diagnostic {
  void error(Token token, String message);
}
