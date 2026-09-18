package com.craftinginterpreters.lox;

final class Interpreter implements Expr.Visitor<Object> {
  Object evaluate(Expr expression) {
    return expression.accept(this);
  }

  @Override
  public Object visitLiteralExpr(Expr.Literal expr) {
    return expr.value;
  }

  @Override
  public Object visitGroupingExpr(Expr.Grouping expr) {
    return evaluate(expr.expression);
  }

  @Override
  public Object visitUnaryExpr(Expr.Unary expr) {
    Object right = evaluate(expr.right);
    return switch (expr.operator.type) {
      case BANG -> !isTruthy(right);
      case MINUS -> {
        checkNumberOperand(expr.operator, right);
        yield -(double) right;
      }
      default -> throw new IllegalStateException("Unexpected unary operator.");
    };
  }

  @Override
  public Object visitBinaryExpr(Expr.Binary expr) {
    Object left = evaluate(expr.left);
    Object right = evaluate(expr.right);

    return switch (expr.operator.type) {
      case MINUS -> {
        checkNumberOperands(expr.operator, left, right);
        yield (double) left - (double) right;
      }
      case SLASH -> {
        checkNumberOperands(expr.operator, left, right);
        if ((double) right == 0.0) {
          throw new RuntimeError(expr.operator, "Cannot divide by zero.");
        }
        yield (double) left / (double) right;
      }
      case STAR -> {
        checkNumberOperands(expr.operator, left, right);
        yield (double) left * (double) right;
      }
      case PLUS -> add(expr.operator, left, right);
      case GREATER -> compare(expr.operator, left, right) > 0;
      case GREATER_EQUAL -> compare(expr.operator, left, right) >= 0;
      case LESS -> compare(expr.operator, left, right) < 0;
      case LESS_EQUAL -> compare(expr.operator, left, right) <= 0;
      case BANG_EQUAL -> !isEqual(left, right);
      case EQUAL_EQUAL -> isEqual(left, right);
      default -> throw new IllegalStateException("Unexpected binary operator.");
    };
  }

  private Object add(Token operator, Object left, Object right) {
    if (left instanceof Double leftNumber && right instanceof Double rightNumber) {
      return leftNumber + rightNumber;
    }
    if (left instanceof String || right instanceof String) {
      return stringify(left) + stringify(right);
    }
    throw new RuntimeError(operator,
        "Operands must be two numbers, or at least one operand must be a string.");
  }

  private int compare(Token operator, Object left, Object right) {
    if (left instanceof Double leftNumber && right instanceof Double rightNumber) {
      return Double.compare(leftNumber, rightNumber);
    }
    if (left instanceof String leftString && right instanceof String rightString) {
      return leftString.compareTo(rightString);
    }
    throw new RuntimeError(operator,
        "Comparison operands must be two numbers or two strings of the same type.");
  }

  static String stringify(Object object) {
    if (object == null) return "nil";
    if (object instanceof Double number) {
      String text = number.toString();
      if (text.endsWith(".0")) return text.substring(0, text.length() - 2);
      return text;
    }
    return object.toString();
  }

  private boolean isTruthy(Object object) {
    if (object == null) return false;
    if (object instanceof Boolean value) return value;
    return true;
  }

  private boolean isEqual(Object left, Object right) {
    if (left == null && right == null) return true;
    if (left == null) return false;
    return left.equals(right);
  }

  private void checkNumberOperand(Token operator, Object operand) {
    if (operand instanceof Double) return;
    throw new RuntimeError(operator, "Operand must be a number.");
  }

  private void checkNumberOperands(Token operator, Object left, Object right) {
    if (left instanceof Double && right instanceof Double) return;
    throw new RuntimeError(operator, "Operands must be numbers.");
  }
}
