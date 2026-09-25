package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static com.craftinginterpreters.lox.TokenType.*;

final class Scanner {
  private static final Map<String, TokenType> KEYWORDS = Map.of(
      "false", FALSE, "nil", NIL, "print", PRINT, "true", TRUE, "var", VAR);

  private final String source;
  private final Diagnostic diagnostic;
  private final List<Token> tokens = new ArrayList<>();
  private int start;
  private int current;
  private int line = 1;

  Scanner(String source, Diagnostic diagnostic) {
    this.source = source;
    this.diagnostic = diagnostic;
  }

  List<Token> scanTokens() {
    while (!isAtEnd()) {
      start = current;
      scanToken();
    }
    tokens.add(new Token(EOF, "", null, line));
    return tokens;
  }

  private void scanToken() {
    char c = advance();
    switch (c) {
      case '(' -> add(LEFT_PAREN);
      case ')' -> add(RIGHT_PAREN);
      case '{' -> add(LEFT_BRACE);
      case '}' -> add(RIGHT_BRACE);
      case ',' -> add(COMMA);
      case '.' -> add(DOT);
      case '-' -> add(MINUS);
      case '+' -> add(PLUS);
      case ';' -> add(SEMICOLON);
      case '*' -> add(STAR);
      case '!' -> add(match('=') ? BANG_EQUAL : BANG);
      case '=' -> add(match('=') ? EQUAL_EQUAL : EQUAL);
      case '<' -> add(match('=') ? LESS_EQUAL : LESS);
      case '>' -> add(match('=') ? GREATER_EQUAL : GREATER);
      case '/' -> {
        if (match('/')) while (peek() != '\n' && !isAtEnd()) advance();
        else add(SLASH);
      }
      case ' ', '\r', '\t' -> { }
      case '\n' -> line++;
      case '"' -> string();
      default -> {
        if (isDigit(c)) number();
        else if (isAlpha(c)) identifier();
        else diagnostic.error(new Token(EOF, String.valueOf(c), null, line),
            "Unexpected character.");
      }
    }
  }

  private void identifier() {
    while (isAlphaNumeric(peek())) advance();
    String text = source.substring(start, current);
    add(KEYWORDS.getOrDefault(text, IDENTIFIER));
  }

  private void number() {
    while (isDigit(peek())) advance();
    if (peek() == '.' && isDigit(peekNext())) {
      advance();
      while (isDigit(peek())) advance();
    }
    add(NUMBER, Double.parseDouble(source.substring(start, current)));
  }

  private void string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n') line++;
      advance();
    }
    if (isAtEnd()) {
      diagnostic.error(new Token(EOF, "", null, line), "Unterminated string.");
      return;
    }
    advance();
    add(STRING, source.substring(start + 1, current - 1));
  }

  private boolean match(char expected) {
    if (isAtEnd() || source.charAt(current) != expected) return false;
    current++;
    return true;
  }

  private char peek() { return isAtEnd() ? '\0' : source.charAt(current); }
  private char peekNext() {
    return current + 1 >= source.length() ? '\0' : source.charAt(current + 1);
  }
  private boolean isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }
  private boolean isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }
  private boolean isDigit(char c) { return c >= '0' && c <= '9'; }
  private boolean isAtEnd() { return current >= source.length(); }
  private char advance() { return source.charAt(current++); }
  private void add(TokenType type) { add(type, null); }
  private void add(TokenType type, Object literal) {
    tokens.add(new Token(type, source.substring(start, current), literal, line));
  }
}
