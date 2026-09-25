package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static com.craftinginterpreters.lox.TokenType.*;

final class Scanner {
  private static final Map<String, TokenType> KEYWORDS = Map.of(
      "break", BREAK, "else", ELSE, "false", FALSE, "if", IF,
      "print", PRINT, "true", TRUE, "while", WHILE);

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
      case ';' -> add(SEMICOLON);
      case ' ', '\r', '\t' -> { }
      case '\n' -> line++;
      case '"' -> string();
      default -> {
        if (isAlpha(c)) identifier();
        else diagnostic.error(new Token(EOF, String.valueOf(c), null, line),
            "Unexpected character.");
      }
    }
  }

  private void identifier() {
    while (isAlphaNumeric(peek())) advance();
    String text = source.substring(start, current);
    TokenType type = KEYWORDS.get(text);
    if (type == null) {
      diagnostic.error(new Token(EOF, text, null, line),
          "Only control-flow keywords are supported by this focused example.");
      return;
    }
    add(type);
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

  private char peek() { return isAtEnd() ? '\0' : source.charAt(current); }
  private boolean isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }
  private boolean isAlphaNumeric(char c) {
    return isAlpha(c) || (c >= '0' && c <= '9');
  }
  private boolean isAtEnd() { return current >= source.length(); }
  private char advance() { return source.charAt(current++); }
  private void add(TokenType type) { add(type, null); }
  private void add(TokenType type, Object literal) {
    tokens.add(new Token(type, source.substring(start, current), literal, line));
  }
}
