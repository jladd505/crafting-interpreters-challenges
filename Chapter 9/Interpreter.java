package com.craftinginterpreters.lox;

import java.util.List;
import java.util.function.Consumer;

final class Interpreter implements Stmt.Visitor<Void> {
  private final Consumer<String> output;

  Interpreter(Consumer<String> output) {
    this.output = output;
  }

  void interpret(List<Stmt> statements) {
    for (Stmt statement : statements) execute(statement);
  }

  private void execute(Stmt statement) {
    statement.accept(this);
  }

  @Override
  public Void visitBlockStmt(Stmt.Block stmt) {
    for (Stmt statement : stmt.statements) execute(statement);
    return null;
  }

  @Override
  public Void visitBreakStmt(Stmt.Break stmt) {
    throw new BreakSignal();
  }

  @Override
  public Void visitIfStmt(Stmt.If stmt) {
    if (stmt.condition) execute(stmt.thenBranch);
    else if (stmt.elseBranch != null) execute(stmt.elseBranch);
    return null;
  }

  @Override
  public Void visitPrintStmt(Stmt.Print stmt) {
    output.accept(stmt.value);
    return null;
  }

  @Override
  public Void visitWhileStmt(Stmt.While stmt) {
    try {
      while (stmt.condition) execute(stmt.body);
    } catch (BreakSignal signal) {
      // The nearest loop consumes the signal, so outer loops keep running.
    }
    return null;
  }
}
