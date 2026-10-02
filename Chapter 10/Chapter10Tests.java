package chapter10;

import java.util.List;

public final class Chapter10Tests {
  private static int tests;

  public static void main(String[] args) {
    var selector = Functions.Selector.smalltalk("insert:at:");
    check(selector.arity() == 2, "Smalltalk selector encodes two arguments");
    check(!selector.equals(Functions.Selector.smalltalk("insert:")), "different arity creates a different selector");

    var named = new Functions.Parser("fun add(a, b) { return a; }").parse();
    check(named instanceof Functions.Named, "named declaration parsed");
    var namedFunction = ((Functions.Named) named).declaration();
    check(namedFunction.name().equals("add") && namedFunction.function().parameters().size() == 2,
        "named declaration retains name and parameters");

    var anonymous = new Functions.Parser("fun (a) { return a; }").parse();
    check(anonymous instanceof Functions.Anonymous, "anonymous function expression parsed");

    var globals = new Functions.Environment();
    globals.define("message", "captured");
    var expression = ((Functions.Anonymous) anonymous).expression();
    var lambda = new Functions.LoxFunction(null, expression, globals);
    var function = new Functions.LoxFunction("identity", expression, globals);
    check(lambda.toString().equals("<fn>"), "anonymous display name");
    check(function.toString().equals("<fn identity>"), "named display name");
    check(function.captured("message").equals("captured"), "function closes over declaration environment");

    expectFailure(() -> Functions.validateFunctionScope(List.of("a"), List.of("a")),
        "local cannot redeclare parameter");
    Functions.validateFunctionScope(List.of("a"), List.of("b"));
    check(true, "different local name is valid");
    System.out.println("Chapter 10: " + tests + " tests passed.");
  }

  private static void check(boolean condition, String message) {
    tests++;
    if (!condition) throw new AssertionError(message);
  }

  private static void expectFailure(Runnable action, String message) {
    tests++;
    try { action.run(); } catch (IllegalArgumentException expected) { return; }
    throw new AssertionError(message);
  }
}

