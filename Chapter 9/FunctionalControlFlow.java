package com.craftinginterpreters.lox;

import java.util.function.Supplier;

final class FunctionalControlFlow {
  interface DynamicBoolean {
    <T> T choose(Supplier<T> whenTrue, Supplier<T> whenFalse);
  }

  static final DynamicBoolean TRUE = new DynamicBoolean() {
    @Override
    public <T> T choose(Supplier<T> whenTrue, Supplier<T> whenFalse) {
      return whenTrue.get();
    }
  };

  static final DynamicBoolean FALSE = new DynamicBoolean() {
    @Override
    public <T> T choose(Supplier<T> whenTrue, Supplier<T> whenFalse) {
      return whenFalse.get();
    }
  };

  interface Step<T> {
    boolean complete();
    T result();
    Step<T> next();
  }

  private record Done<T>(T result) implements Step<T> {
    @Override public boolean complete() { return true; }
    @Override public Step<T> next() { return this; }
  }

  private record Call<T>(Supplier<Step<T>> continuation) implements Step<T> {
    @Override public boolean complete() { return false; }
    @Override public T result() { throw new IllegalStateException("Call is not complete."); }
    @Override public Step<T> next() { return continuation.get(); }
  }

  static <T> T trampoline(Step<T> step) {
    Step<T> current = step;
    while (!current.complete()) current = current.next();
    return current.result();
  }

  static Step<Long> sumTail(long remaining, long total) {
    if (remaining == 0) return new Done<>(total);
    return new Call<>(() -> sumTail(remaining - 1, total + remaining));
  }

  private FunctionalControlFlow() { }
}
