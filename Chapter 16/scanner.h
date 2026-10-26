#ifndef CHAPTER16_SCANNER_H
#define CHAPTER16_SCANNER_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  TOKEN_INTERPOLATION,
  TOKEN_STRING,
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_LESS,
  TOKEN_GREATER,
  TOKEN_SHIFT_RIGHT,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  char lexeme[96];
  int line;
} Token;

size_t scanTokens(const char* source, Token* tokens, size_t capacity);
const char* tokenTypeName(TokenType type);

typedef struct {
  const Token* tokens;
  size_t count;
  size_t current;
  int pendingGreater;
} GenericCursor;

Token genericNext(GenericCursor* cursor);
bool isContextualKeyword(Token token, const char* word, bool grammarExpectsIt);

#endif

