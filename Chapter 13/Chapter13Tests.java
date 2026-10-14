package chapter13;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public final class Chapter13Tests {
  private static int tests;

  public static void main(String[] args) {
    traitsComposeAndRejectCollisions();
    betaDispatchUsesTopDownInnerChain();
    superclassCanPreventRefinement();
    arraysProvideContiguousIndexedStorage();
    System.out.println("Chapter 13: " + tests + " tests passed.");
  }

  private static void traitsComposeAndRejectCollisions() {
    var printable = new Inheritance.Trait("Printable", Map.of("print", receiver -> "printed"));
    var named = new Inheritance.Trait("Named", Map.of("name", receiver -> "name"));
    var methods = Inheritance.composeTraits(List.of(printable, named), Map.of());
    check(methods.size() == 2, "two traits compose into a flat method set");
    check(methods.get("print").call(new Object()).equals("printed"), "composed method callable");

    var duplicate = new Inheritance.Trait("Duplicate", Map.of("print", receiver -> "other"));
    expectFailure(() -> Inheritance.composeTraits(List.of(printable, duplicate), Map.of()),
        "trait collision rejected");
    expectFailure(() -> Inheritance.composeTraits(List.of(printable), Map.of("print", receiver -> "own")),
        "class body cannot silently override trait collision");
  }

  private static void betaDispatchUsesTopDownInnerChain() {
    List<String> output = new ArrayList<>();
    var doughnut = new Inheritance.BetaClass("Doughnut", null, Map.of(
        "cook", (receiver, inner) -> {
          output.add("Fry until golden brown.");
          inner.run();
          output.add("Place in a nice box.");
        }));
    var boston = new Inheritance.BetaClass("BostonCream", doughnut, Map.of(
        "cook", (receiver, inner) -> output.add("Pipe full of custard and coat with chocolate.")));
    boston.instantiate().call("cook");
    check(output.equals(List.of(
        "Fry until golden brown.",
        "Pipe full of custard and coat with chocolate.",
        "Place in a nice box.")), "inner invokes the next subclass implementation");
  }

  private static void superclassCanPreventRefinement() {
    List<String> output = new ArrayList<>();
    var base = new Inheritance.BetaClass("Base", null,
        Map.of("run", (receiver, inner) -> output.add("base")));
    var child = new Inheritance.BetaClass("Child", base,
        Map.of("run", (receiver, inner) -> output.add("child")));
    child.instantiate().call("run");
    check(output.equals(List.of("base")), "subclass body runs only when superclass calls inner");
  }

  private static void arraysProvideContiguousIndexedStorage() {
    var array = new Inheritance.LoxArray(3);
    check(array.length() == 3, "array reports length");
    check(array.toString().equals("[null, null, null]"), "array initializes elements to nil-like null");
    check(array.set(1, "new").equals("new") && array.get(1).equals("new"), "array set and get work");
    expectFailure(() -> array.get(3), "array bounds checked");
  }

  private static void check(boolean condition, String message) {
    tests++;
    if (!condition) throw new AssertionError(message);
  }

  private static void expectFailure(Runnable action, String message) {
    tests++;
    try { action.run(); } catch (RuntimeException expected) { return; }
    throw new AssertionError(message);
  }
}
