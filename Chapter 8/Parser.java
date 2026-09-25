package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;

import static com.craftinginterpreters.lox.TokenType.*;

final class Parser {
  @SuppressWarnings("serial")
  private static final class ParseError extends RuntimeException { }

  private final List<Token> tokens;
  private final Diagnostic diagnostic;
  private int current;

  Parser(List<Token> tokens, Diagnostic diagnostic) {
    this.tokens = tokens;
    this.diagnostic = diagnostic;
  }

  List<Stmt> parse() {
    List<Stmt> statements = new ArrayList<>();
    while (!isAtEnd()) statements.add(declaration());
    return statements;
  }

  ReplUnit parseReplUnit() {
    boolean statementForm = check(VAR) || check(PRINT) || check(LEFT_BRACE)
        || tokens.stream().anyMatch(token -> token.type == SEMICOLON);
    if (statementForm) return new ReplUnit.StatementInput(parse());

    Expr value = expression();
    consume(EOF, "Expect end of expression.");
    return new ReplUnit.ExpressionInput(value);
  }

  private Stmt declaration() {
    try {
      if (match(VAR)) return varDeclaration();
      return statement();
    } catch (ParseError error) {
      synchronize();
      return new Stmt.Expression(new Expr.Literal(null));
    }
  }

  private Stmt varDeclaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");
    Expr initializer = null;
    if (match(EQUAL)) initializer = expression();
    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return new Stmt.Var(name, initializer);
  }

  private Stmt statement() {
    if (match(PRINT)) return printStatement();
    if (match(LEFT_BRACE)) return new Stmt.Block(block());
    return expressionStatement();
  }

  private Stmt printStatement() {
    Expr value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return new Stmt.Print(value);
  }

  private Stmt expressionStatement() {
    Expr value = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return new Stmt.Expression(value);
  }

  private List<Stmt> block() {
    List<Stmt> statements = new ArrayList<>();
    while (!check(RIGHT_BRACE) && !isAtEnd()) statements.add(declaration());
    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
  }

  private Expr expression() { return assignment(); }

  private Expr assignment() {
    Expr expr = equality();
    if (match(EQUAL)) {
      Token equals = previous();
      Expr value = assignment();
      if (expr instanceof Expr.Variable variable) {
        return new Expr.Assign(variable.name, value);
      }
      error(equals, "Invalid assignment target.");
    }
    return expr;
  }

  private Expr equality() {
    Expr expr = comparison();
    while (match(BANG_EQUAL, EQUAL_EQUAL)) {
      Token operator = previous();
      expr = new Expr.Binary(expr, operator, comparison());
    }
    return expr;
  }

  private Expr comparison() {
    Expr expr = term();
    while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
      Token operator = previous();
      expr = new Expr.Binary(expr, operator, term());
    }
    return expr;
  }

  private Expr term() {
    Expr expr = factor();
    while (match(MINUS, PLUS)) {
      Token operator = previous();
      expr = new Expr.Binary(expr, operator, factor());
    }
    return expr;
  }

  private Expr factor() {
    Expr expr = unary();
    while (match(SLASH, STAR)) {
      Token operator = previous();
      expr = new Expr.Binary(expr, operator, unary());
    }
    return expr;
  }

  private Expr unary() {
    if (match(BANG, MINUS)) return new Expr.Unary(previous(), unary());
    return primary();
  }

  private Expr primary() {
    if (match(FALSE)) return new Expr.Literal(false);
    if (match(TRUE)) return new Expr.Literal(true);
    if (match(NIL)) return new Expr.Literal(null);
    if (match(NUMBER, STRING)) return new Expr.Literal(previous().literal);
    if (match(IDENTIFIER)) return new Expr.Variable(previous());
    if (match(LEFT_PAREN)) {
      Expr expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return new Expr.Grouping(expr);
    }
    throw error(peek(), "Expect expression.");
  }

  private boolean match(TokenType... types) {
    for (TokenType type : types) {
      if (check(type)) { advance(); return true; }
    }
    return false;
  }
  private Token consume(TokenType type, String message) {
    if (check(type)) return advance();
    throw error(peek(), message);
  }
  private ParseError error(Token token, String message) {
    diagnostic.error(token, message);
    return new ParseError();
  }
  private void synchronize() {
    advance();
    while (!isAtEnd()) {
      if (previous().type == SEMICOLON) return;
      if (peek().type == VAR || peek().type == PRINT) return;
      advance();
    }
  }
  private boolean check(TokenType type) {
    if (isAtEnd()) return type == EOF;
    return peek().type == type;
  }
  private Token advance() {
    if (!isAtEnd()) current++;
    return previous();
  }
  private boolean isAtEnd() { return peek().type == EOF; }
  private Token peek() { return tokens.get(current); }
  private Token previous() { return tokens.get(current - 1); }
}
