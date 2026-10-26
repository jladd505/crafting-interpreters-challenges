#include "scanner.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char* source;
  size_t current;
  int line;
  Token* tokens;
  size_t count;
  size_t capacity;
} Scanner;

static void addSlice(Scanner* scanner, TokenType type, size_t start, size_t end) {
  if (scanner->count >= scanner->capacity) return;
  Token* token = &scanner->tokens[scanner->count++];
  token->type = type;
  token->line = scanner->line;
  size_t length = end - start;
  if (length >= sizeof(token->lexeme)) length = sizeof(token->lexeme) - 1;
  memcpy(token->lexeme, scanner->source + start, length);
  token->lexeme[length] = '\0';
}

static void addText(Scanner* scanner, TokenType type, const char* text) {
  if (scanner->count >= scanner->capacity) return;
  Token* token = &scanner->tokens[scanner->count++];
  token->type = type;
  token->line = scanner->line;
  snprintf(token->lexeme, sizeof(token->lexeme), "%s", text);
}

static void scanExpression(Scanner* scanner);

static void scanString(Scanner* scanner) {
  size_t segment = scanner->current;
  for (;;) {
    char c = scanner->source[scanner->current];
    if (c == '\0') {
      addText(scanner, TOKEN_ERROR, "Unterminated string.");
      return;
    }
    if (c == '\n') scanner->line++;
    if (c == '"') {
      addSlice(scanner, TOKEN_STRING, segment, scanner->current);
      scanner->current++;
      return;
    }
    if (c == '$' && scanner->source[scanner->current + 1] == '{') {
      addSlice(scanner, TOKEN_INTERPOLATION, segment, scanner->current);
      scanner->current += 2;
      scanExpression(scanner);
      segment = scanner->current;
      continue;
    }
    scanner->current++;
  }
}

static void scanIdentifier(Scanner* scanner) {
  size_t start = scanner->current++;
  while (isalnum((unsigned char)scanner->source[scanner->current]) ||
         scanner->source[scanner->current] == '_') {
    scanner->current++;
  }
  addSlice(scanner, TOKEN_IDENTIFIER, start, scanner->current);
}

static void scanNumber(Scanner* scanner) {
  size_t start = scanner->current++;
  while (isdigit((unsigned char)scanner->source[scanner->current])) scanner->current++;
  if (scanner->source[scanner->current] == '.' &&
      isdigit((unsigned char)scanner->source[scanner->current + 1])) {
    scanner->current++;
    while (isdigit((unsigned char)scanner->source[scanner->current])) scanner->current++;
  }
  addSlice(scanner, TOKEN_NUMBER, start, scanner->current);
}

static void scanExpression(Scanner* scanner) {
  int braces = 0;
  for (;;) {
    char c = scanner->source[scanner->current];
    if (c == '\0') {
      addText(scanner, TOKEN_ERROR, "Unterminated interpolation.");
      return;
    }
    if (c == '}') {
      if (braces == 0) {
        scanner->current++;
        return;
      }
      braces--;
      addText(scanner, TOKEN_RIGHT_BRACE, "}");
      scanner->current++;
      continue;
    }
    if (c == '{') {
      braces++;
      addText(scanner, TOKEN_LEFT_BRACE, "{");
      scanner->current++;
      continue;
    }
    if (c == '"') {
      scanner->current++;
      scanString(scanner);
      continue;
    }
    if (c == ' ' || c == '\r' || c == '\t') {
      scanner->current++;
      continue;
    }
    if (c == '\n') {
      scanner->line++;
      scanner->current++;
      continue;
    }
    if (isalpha((unsigned char)c) || c == '_') {
      scanIdentifier(scanner);
      continue;
    }
    if (isdigit((unsigned char)c)) {
      scanNumber(scanner);
      continue;
    }
    TokenType type = TOKEN_ERROR;
    const char* text = "Unexpected character.";
    if (c == '+') { type = TOKEN_PLUS; text = "+"; }
    else if (c == '-') { type = TOKEN_MINUS; text = "-"; }
    else if (c == '*') { type = TOKEN_STAR; text = "*"; }
    else if (c == '/') { type = TOKEN_SLASH; text = "/"; }
    else if (c == '<') { type = TOKEN_LESS; text = "<"; }
    else if (c == '>' && scanner->source[scanner->current + 1] == '>') {
      type = TOKEN_SHIFT_RIGHT; text = ">>"; scanner->current++;
    } else if (c == '>') { type = TOKEN_GREATER; text = ">"; }
    addText(scanner, type, text);
    scanner->current++;
  }
}

size_t scanTokens(const char* source, Token* tokens, size_t capacity) {
  Scanner scanner = {source, 0, 1, tokens, 0, capacity};
  while (source[scanner.current] != '\0') {
    char c = source[scanner.current];
    if (c == '"') {
      scanner.current++;
      scanString(&scanner);
    } else if (isalpha((unsigned char)c) || c == '_') {
      scanIdentifier(&scanner);
    } else if (isdigit((unsigned char)c)) {
      scanNumber(&scanner);
    } else if (c == '>' && source[scanner.current + 1] == '>') {
      addText(&scanner, TOKEN_SHIFT_RIGHT, ">>");
      scanner.current += 2;
    } else if (c == '>') {
      addText(&scanner, TOKEN_GREATER, ">");
      scanner.current++;
    } else {
      scanner.current++;
    }
  }
  addText(&scanner, TOKEN_EOF, "");
  return scanner.count;
}

Token genericNext(GenericCursor* cursor) {
  Token result = {TOKEN_EOF, "", 0};
  if (cursor->pendingGreater > 0) {
    cursor->pendingGreater--;
    result.type = TOKEN_GREATER;
    snprintf(result.lexeme, sizeof(result.lexeme), ">");
    return result;
  }
  if (cursor->current >= cursor->count) return result;
  result = cursor->tokens[cursor->current++];
  if (result.type == TOKEN_SHIFT_RIGHT) {
    result.type = TOKEN_GREATER;
    snprintf(result.lexeme, sizeof(result.lexeme), ">");
    cursor->pendingGreater = 1;
  }
  return result;
}

bool isContextualKeyword(Token token, const char* word, bool grammarExpectsIt) {
  return grammarExpectsIt && token.type == TOKEN_IDENTIFIER && strcmp(token.lexeme, word) == 0;
}

const char* tokenTypeName(TokenType type) {
  static const char* names[] = {
    "INTERPOLATION", "STRING", "IDENTIFIER", "NUMBER", "PLUS", "MINUS", "STAR",
    "SLASH", "LESS", "GREATER", "SHIFT_RIGHT", "LEFT_BRACE", "RIGHT_BRACE", "ERROR", "EOF"
  };
  return names[type];
}

