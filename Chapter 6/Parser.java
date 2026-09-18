package com.craftinginterpreters.lox;

import java.util.List;

import static com.craftinginterpreters.lox.TokenType.*;

final class Parser {
  private static final class ParseError extends RuntimeException {
    private static final long serialVersionUID = 1L;
  }

  private final List<Token> tokens;
  private final Diagnostic diagnostic;
  private int current;

  Parser(List<Token> tokens, Diagnostic diagnostic) {
    this.tokens = tokens;
    this.diagnostic = diagnostic;
  }

  Expr parse() {
    try {
      Expr expression = expression();
      consume(EOF, "Expect end of expression.");
      return expression;
    } catch (ParseError error) {
      return null;
    }
  }

  // expression  -> comma ;
  // comma       -> conditional ( "," conditional )* ;
  // conditional -> equality ( "?" expression ":" conditional )? ;
  private Expr expression() {
    return comma();
  }

  private Expr comma() {
    Expr expr = conditional();
    while (match(COMMA)) {
      Token operator = previous();
      Expr right = conditional();
      expr = new Expr.Binary(expr, operator, right);
    }
    return expr;
  }

  private Expr conditional() {
    Expr expr = equality();
    if (match(QUESTION)) {
      Expr thenBranch = expression();
      consume(COLON, "Expect ':' after the then branch of a conditional expression.");
      Expr elseBranch = conditional();
      expr = new Expr.Conditional(expr, thenBranch, elseBranch);
    }
    return expr;
  }

  private Expr equality() {
    Expr expr = comparison();
    while (match(BANG_EQUAL, EQUAL_EQUAL)) {
      Token operator = previous();
      Expr right = comparison();
      expr = new Expr.Binary(expr, operator, right);
    }
    return expr;
  }

  private Expr comparison() {
    Expr expr = term();
    while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
      Token operator = previous();
      Expr right = term();
      expr = new Expr.Binary(expr, operator, right);
    }
    return expr;
  }

  private Expr term() {
    Expr expr = factor();
    while (match(MINUS, PLUS)) {
      Token operator = previous();
      Expr right = factor();
      expr = new Expr.Binary(expr, operator, right);
    }
    return expr;
  }

  private Expr factor() {
    Expr expr = unary();
    while (match(SLASH, STAR)) {
      Token operator = previous();
      Expr right = unary();
      expr = new Expr.Binary(expr, operator, right);
    }
    return expr;
  }

  private Expr unary() {
    if (match(BANG, MINUS)) {
      Token operator = previous();
      return new Expr.Unary(operator, unary());
    }
    return primary();
  }

  private Expr primary() {
    if (match(FALSE)) return new Expr.Literal(false);
    if (match(TRUE)) return new Expr.Literal(true);
    if (match(NIL)) return new Expr.Literal(null);
    if (match(NUMBER, STRING)) return new Expr.Literal(previous().literal);

    if (match(LEFT_PAREN)) {
      Expr expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return new Expr.Grouping(expr);
    }

    if (match(COMMA)) return missingLeft(previous(), this::conditional);
    if (match(BANG_EQUAL, EQUAL_EQUAL)) return missingLeft(previous(), this::comparison);
    if (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
      return missingLeft(previous(), this::term);
    }
    if (match(MINUS, PLUS)) return missingLeft(previous(), this::factor);
    if (match(SLASH, STAR)) return missingLeft(previous(), this::unary);

    throw error(peek(), "Expect expression.");
  }

  private Expr missingLeft(Token operator, java.util.function.Supplier<Expr> rightParser) {
    ParseError parseError = error(operator,
        "Binary operator '" + operator.lexeme + "' is missing a left-hand operand.");
    rightParser.get();
    throw parseError;
  }

  private Token consume(TokenType type, String message) {
    if (check(type)) return advance();
    throw error(peek(), message);
  }

  private ParseError error(Token token, String message) {
    diagnostic.error(token, message);
    return new ParseError();
  }

  private boolean match(TokenType... types) {
    for (TokenType type : types) {
      if (check(type)) {
        advance();
        return true;
      }
    }
    return false;
  }

  private boolean check(TokenType type) {
    if (isAtEnd()) return type == EOF;
    return peek().type == type;
  }

  private Token advance() {
    if (!isAtEnd()) current++;
    return previous();
  }

  private boolean isAtEnd() {
    return peek().type == EOF;
  }

  private Token peek() {
    return tokens.get(current);
  }

  private Token previous() {
    return tokens.get(current - 1);
  }
}
