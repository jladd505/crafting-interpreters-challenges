package com.craftinginterpreters.lox;

import java.util.List;

abstract class Stmt {
  interface Visitor<R> {
    R visitBlockStmt(Block stmt);
    R visitBreakStmt(Break stmt);
    R visitIfStmt(If stmt);
    R visitPrintStmt(Print stmt);
    R visitWhileStmt(While stmt);
  }

  static final class Block extends Stmt {
    final List<Stmt> statements;
    Block(List<Stmt> statements) { this.statements = statements; }
    @Override <R> R accept(Visitor<R> visitor) { return visitor.visitBlockStmt(this); }
  }

  static final class Break extends Stmt {
    final Token keyword;
    Break(Token keyword) { this.keyword = keyword; }
    @Override <R> R accept(Visitor<R> visitor) { return visitor.visitBreakStmt(this); }
  }

  static final class If extends Stmt {
    final boolean condition;
    final Stmt thenBranch;
    final Stmt elseBranch;
    If(boolean condition, Stmt thenBranch, Stmt elseBranch) {
      this.condition = condition; this.thenBranch = thenBranch; this.elseBranch = elseBranch;
    }
    @Override <R> R accept(Visitor<R> visitor) { return visitor.visitIfStmt(this); }
  }

  static final class Print extends Stmt {
    final String value;
    Print(String value) { this.value = value; }
    @Override <R> R accept(Visitor<R> visitor) { return visitor.visitPrintStmt(this); }
  }

  static final class While extends Stmt {
    final boolean condition;
    final Stmt body;
    While(boolean condition, Stmt body) { this.condition = condition; this.body = body; }
    @Override <R> R accept(Visitor<R> visitor) { return visitor.visitWhileStmt(this); }
  }

  abstract <R> R accept(Visitor<R> visitor);
}
