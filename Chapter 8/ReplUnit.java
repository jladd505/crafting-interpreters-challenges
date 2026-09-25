package com.craftinginterpreters.lox;

import java.util.List;

abstract class ReplUnit {
  static final class ExpressionInput extends ReplUnit {
    final Expr expression;
    ExpressionInput(Expr expression) { this.expression = expression; }
  }

  static final class StatementInput extends ReplUnit {
    final List<Stmt> statements;
    StatementInput(List<Stmt> statements) { this.statements = statements; }
  }
}
