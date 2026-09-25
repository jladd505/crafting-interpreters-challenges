package com.craftinginterpreters.lox;

import java.util.List;
import java.util.function.Consumer;

final class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void> {
  private final Consumer<String> output;
  private Environment environment = new Environment();

  Interpreter(Consumer<String> output) {
    this.output = output;
  }

  void interpret(List<Stmt> statements) {
    for (Stmt statement : statements) execute(statement);
  }

  Object evaluate(Expr expression) {
    return expression.accept(this);
  }

  private void execute(Stmt statement) {
    statement.accept(this);
  }

  @Override
  public Void visitBlockStmt(Stmt.Block stmt) {
    executeBlock(stmt.statements, new Environment(environment));
    return null;
  }

  void executeBlock(List<Stmt> statements, Environment blockEnvironment) {
    Environment previous = environment;
    try {
      environment = blockEnvironment;
      for (Stmt statement : statements) execute(statement);
    } finally {
      environment = previous;
    }
  }

  @Override
  public Void visitExpressionStmt(Stmt.Expression stmt) {
    evaluate(stmt.expression);
    return null;
  }

  @Override
  public Void visitPrintStmt(Stmt.Print stmt) {
    output.accept(stringify(evaluate(stmt.expression)));
    return null;
  }

  @Override
  public Void visitVarStmt(Stmt.Var stmt) {
    // The initializer is intentionally evaluated before the new binding exists.
    // This preserves the book's behavior for: var a = 1; { var a = a + 2; }
    Object value = stmt.initializer == null
        ? Environment.UNINITIALIZED
        : evaluate(stmt.initializer);
    environment.define(stmt.name.lexeme, value);
    return null;
  }

  @Override
  public Object visitAssignExpr(Expr.Assign expr) {
    Object value = evaluate(expr.value);
    environment.assign(expr.name, value);
    return value;
  }

  @Override
  public Object visitBinaryExpr(Expr.Binary expr) {
    Object left = evaluate(expr.left);
    Object right = evaluate(expr.right);
    return switch (expr.operator.type) {
      case MINUS -> numbers(expr.operator, left, right, (a, b) -> a - b);
      case SLASH -> numbers(expr.operator, left, right, (a, b) -> a / b);
      case STAR -> numbers(expr.operator, left, right, (a, b) -> a * b);
      case PLUS -> {
        if (left instanceof Double a && right instanceof Double b) yield a + b;
        if (left instanceof String a && right instanceof String b) yield a + b;
        throw new RuntimeError(expr.operator,
            "Operands must be two numbers or two strings.");
      }
      case GREATER -> compare(expr.operator, left, right) > 0;
      case GREATER_EQUAL -> compare(expr.operator, left, right) >= 0;
      case LESS -> compare(expr.operator, left, right) < 0;
      case LESS_EQUAL -> compare(expr.operator, left, right) <= 0;
      case BANG_EQUAL -> !isEqual(left, right);
      case EQUAL_EQUAL -> isEqual(left, right);
      default -> throw new IllegalStateException("Unexpected binary operator.");
    };
  }

  @Override
  public Object visitGroupingExpr(Expr.Grouping expr) {
    return evaluate(expr.expression);
  }

  @Override
  public Object visitLiteralExpr(Expr.Literal expr) {
    return expr.value;
  }

  @Override
  public Object visitUnaryExpr(Expr.Unary expr) {
    Object right = evaluate(expr.right);
    return switch (expr.operator.type) {
      case BANG -> !isTruthy(right);
      case MINUS -> {
        if (!(right instanceof Double number)) {
          throw new RuntimeError(expr.operator, "Operand must be a number.");
        }
        yield -number;
      }
      default -> throw new IllegalStateException("Unexpected unary operator.");
    };
  }

  @Override
  public Object visitVariableExpr(Expr.Variable expr) {
    return environment.get(expr.name);
  }

  private interface NumberOperation { double apply(double left, double right); }
  private Object numbers(Token token, Object left, Object right, NumberOperation operation) {
    if (left instanceof Double a && right instanceof Double b) return operation.apply(a, b);
    throw new RuntimeError(token, "Operands must be numbers.");
  }
  private int compare(Token token, Object left, Object right) {
    if (left instanceof Double a && right instanceof Double b) return Double.compare(a, b);
    throw new RuntimeError(token, "Operands must be numbers.");
  }
  private boolean isTruthy(Object value) {
    if (value == null) return false;
    if (value instanceof Boolean bool) return bool;
    return true;
  }
  private boolean isEqual(Object left, Object right) {
    if (left == null) return right == null;
    return left.equals(right);
  }
  static String stringify(Object value) {
    if (value == null) return "nil";
    if (value instanceof Double number) {
      String text = number.toString();
      return text.endsWith(".0") ? text.substring(0, text.length() - 2) : text;
    }
    return value.toString();
  }
}
