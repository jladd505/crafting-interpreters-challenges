#include "pratt.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum {
  T_NUMBER, T_NAME, T_LEFT_PAREN, T_RIGHT_PAREN, T_PLUS, T_MINUS, T_STAR, T_SLASH,
  T_QUESTION, T_COLON, T_EOF, T_ERROR
} TokenType;

typedef struct { TokenType type; char text[32]; } Token;
typedef struct { char text[512]; } Expr;
typedef struct { const char* source; size_t offset; Token previous; Token current; bool error; } Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,
  PREC_CONDITIONAL,
  PREC_TERM,
  PREC_FACTOR,
  PREC_UNARY,
  PREC_PRIMARY
} Precedence;

static Token scanToken(Parser* parser) {
  while (isspace((unsigned char)parser->source[parser->offset])) parser->offset++;
  Token token = {T_ERROR, ""};
  char c = parser->source[parser->offset++];
  if (c == '\0') { token.type = T_EOF; return token; }
  if (isdigit((unsigned char)c)) {
    size_t start = parser->offset - 1;
    while (isdigit((unsigned char)parser->source[parser->offset])) parser->offset++;
    size_t length = parser->offset - start;
    if (length >= sizeof(token.text)) length = sizeof(token.text) - 1;
    memcpy(token.text, parser->source + start, length);
    token.text[length] = '\0';
    token.type = T_NUMBER;
    return token;
  }
  if (isalpha((unsigned char)c) || c == '_') {
    size_t start = parser->offset - 1;
    while (isalnum((unsigned char)parser->source[parser->offset]) || parser->source[parser->offset] == '_') parser->offset++;
    size_t length = parser->offset - start;
    if (length >= sizeof(token.text)) length = sizeof(token.text) - 1;
    memcpy(token.text, parser->source + start, length);
    token.text[length] = '\0';
    token.type = T_NAME;
    return token;
  }
  token.text[0] = c; token.text[1] = '\0';
  switch (c) {
    case '(': token.type = T_LEFT_PAREN; break;
    case ')': token.type = T_RIGHT_PAREN; break;
    case '+': token.type = T_PLUS; break;
    case '-': token.type = T_MINUS; break;
    case '*': token.type = T_STAR; break;
    case '/': token.type = T_SLASH; break;
    case '?': token.type = T_QUESTION; break;
    case ':': token.type = T_COLON; break;
    default: token.type = T_ERROR; break;
  }
  return token;
}

static void advance(Parser* parser) { parser->previous = parser->current; parser->current = scanToken(parser); }
static Expr parsePrecedence(Parser* parser, Precedence precedence);

static Expr prefix(Parser* parser) {
  Expr result = {{0}};
  Token token = parser->previous;
  if (token.type == T_NUMBER || token.type == T_NAME) {
    snprintf(result.text, sizeof(result.text), "%s", token.text);
  } else if (token.type == T_MINUS || token.type == T_PLUS) {
    Expr right = parsePrecedence(parser, PREC_UNARY);
    snprintf(result.text, sizeof(result.text), "(%s %s)", token.text, right.text);
  } else if (token.type == T_LEFT_PAREN) {
    result = parsePrecedence(parser, PREC_ASSIGNMENT);
    if (parser->current.type != T_RIGHT_PAREN) parser->error = true;
    else advance(parser);
  } else {
    parser->error = true;
  }
  return result;
}

static Precedence infixPrecedence(TokenType type) {
  if (type == T_QUESTION) return PREC_CONDITIONAL;
  if (type == T_PLUS || type == T_MINUS) return PREC_TERM;
  if (type == T_STAR || type == T_SLASH) return PREC_FACTOR;
  return PREC_NONE;
}

static Expr parsePrecedence(Parser* parser, Precedence precedence) {
  advance(parser);
  Expr left = prefix(parser);
  while (!parser->error && precedence <= infixPrecedence(parser->current.type)) {
    advance(parser);
    Token operatorToken = parser->previous;
    if (operatorToken.type == T_QUESTION) {
      Expr thenBranch = parsePrecedence(parser, PREC_ASSIGNMENT);
      if (parser->current.type != T_COLON) { parser->error = true; break; }
      advance(parser);
      Expr elseBranch = parsePrecedence(parser, PREC_CONDITIONAL);
      Expr combined = {{0}};
      snprintf(combined.text, sizeof(combined.text), "(?: %.150s %.150s %.150s)", left.text, thenBranch.text, elseBranch.text);
      left = combined;
    } else {
      Precedence operatorPrecedence = infixPrecedence(operatorToken.type);
      Expr right = parsePrecedence(parser, (Precedence)(operatorPrecedence + 1));
      Expr combined = {{0}};
      snprintf(combined.text, sizeof(combined.text), "(%s %.220s %.220s)", operatorToken.text, left.text, right.text);
      left = combined;
    }
  }
  return left;
}

bool parseExpression(const char* source, char* output, size_t capacity) {
  Parser parser = {source, 0, {T_ERROR, ""}, {T_ERROR, ""}, false};
  parser.current = scanToken(&parser);
  Expr expression = parsePrecedence(&parser, PREC_ASSIGNMENT);
  if (parser.current.type != T_EOF) parser.error = true;
  if (capacity > 0) snprintf(output, capacity, "%s", expression.text);
  return !parser.error;
}

TokenRoles rolesFor(char token) {
  switch (token) {
    case '-': case '+': case '*': case '&': case '(': case '[': return (TokenRoles){true, true};
    default: return (TokenRoles){false, false};
  }
}

const char* challengeTrace(void) {
  return
    "expression\n"
    "  parsePrecedence(ASSIGNMENT)\n"
    "    grouping\n"
    "      expression\n"
    "        parsePrecedence(ASSIGNMENT)\n"
    "          unary(-)\n"
    "            parsePrecedence(UNARY)\n"
    "              number(1)\n"
    "          binary(+)\n"
    "            parsePrecedence(FACTOR)\n"
    "              number(2)\n"
    "    binary(*)\n"
    "      parsePrecedence(UNARY)\n"
    "        number(3)\n"
    "    binary(-)\n"
    "      parsePrecedence(FACTOR)\n"
    "        unary(-)\n"
    "          parsePrecedence(UNARY)\n"
    "            number(4)\n";
}

