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
  private int loopDepth;

  Parser(List<Token> tokens, Diagnostic diagnostic) {
    this.tokens = tokens;
    this.diagnostic = diagnostic;
  }

  List<Stmt> parse() {
    List<Stmt> statements = new ArrayList<>();
    while (!isAtEnd()) statements.add(statement());
    return statements;
  }

  private Stmt statement() {
    if (match(BREAK)) return breakStatement();
    if (match(IF)) return ifStatement();
    if (match(PRINT)) return printStatement();
    if (match(WHILE)) return whileStatement();
    if (match(LEFT_BRACE)) return new Stmt.Block(block());
    throw error(peek(), "Expect statement.");
  }

  private Stmt breakStatement() {
    Token keyword = previous();
    if (loopDepth == 0) {
      throw error(keyword, "Cannot use 'break' outside of a loop.");
    }
    consume(SEMICOLON, "Expect ';' after 'break'.");
    return new Stmt.Break(keyword);
  }

  private Stmt ifStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    boolean condition = booleanLiteral();
    consume(RIGHT_PAREN, "Expect ')' after condition.");
    Stmt thenBranch = statement();
    Stmt elseBranch = match(ELSE) ? statement() : null;
    return new Stmt.If(condition, thenBranch, elseBranch);
  }

  private Stmt whileStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    boolean condition = booleanLiteral();
    consume(RIGHT_PAREN, "Expect ')' after condition.");

    loopDepth++;
    try {
      return new Stmt.While(condition, statement());
    } finally {
      loopDepth--;
    }
  }

  private Stmt printStatement() {
    Token value = consume(STRING, "Expect a string after 'print'.");
    consume(SEMICOLON, "Expect ';' after value.");
    return new Stmt.Print((String) value.literal);
  }

  private List<Stmt> block() {
    List<Stmt> statements = new ArrayList<>();
    while (!check(RIGHT_BRACE) && !isAtEnd()) statements.add(statement());
    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
  }

  private boolean booleanLiteral() {
    if (match(TRUE)) return true;
    if (match(FALSE)) return false;
    throw error(peek(), "Expect a boolean condition in this focused example.");
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
