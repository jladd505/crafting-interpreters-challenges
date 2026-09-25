package com.craftinginterpreters.lox;

@SuppressWarnings("serial")
final class BreakSignal extends RuntimeException {
  BreakSignal() {
    super(null, null, false, false);
  }
}
