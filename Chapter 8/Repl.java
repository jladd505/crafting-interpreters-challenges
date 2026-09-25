package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;

final class Repl {
  private final List<String> output = new ArrayList<>();
  private final Interpreter interpreter = new Interpreter(output::add);

  List<String> runLine(String source) {
    List<String> errors = new ArrayList<>();
    Diagnostic diagnostic = (token, message) ->
        errors.add("[line " + token.line + "] " + message);
    List<Token> tokens = new Scanner(source, diagnostic).scanTokens();
    ReplUnit unit = new Parser(tokens, diagnostic).parseReplUnit();
    if (!errors.isEmpty()) throw new IllegalArgumentException(String.join("\n", errors));

    int outputStart = output.size();
    if (unit instanceof ReplUnit.ExpressionInput input) {
      output.add(Interpreter.stringify(interpreter.evaluate(input.expression)));
    } else if (unit instanceof ReplUnit.StatementInput input) {
      interpreter.interpret(input.statements);
    }
    return List.copyOf(output.subList(outputStart, output.size()));
  }
}
