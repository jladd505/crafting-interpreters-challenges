package chapter11;

import java.util.List;

public final class Chapter11Tests {
  private static int tests;

  public static void main(String[] args) {
    unusedLocalIsReported();
    assignmentIsNotARead();
    readMarksLocalUsed();
    ownInitializerIsRejected();
    lexicalCoordinatesAndSlotsWork();
    System.out.println("Chapter 11: " + tests + " tests passed.");
  }

  private static void unusedLocalIsReported() {
    var resolver = new Resolving.Resolver();
    resolver.resolve(new Resolving.Block(List.of(new Resolving.Var("a", new Resolving.Literal(1), 10))));
    check(resolver.errors().stream().anyMatch(e -> e.contains("'a' is not used")), "unused local reported");
  }

  private static void assignmentIsNotARead() {
    var resolver = new Resolving.Resolver();
    resolver.resolve(new Resolving.Block(List.of(
        new Resolving.Var("a", new Resolving.Literal(1), 1),
        new Resolving.Expression(new Resolving.Assign("a", new Resolving.Literal(2), 2)))));
    check(resolver.errors().stream().anyMatch(e -> e.contains("not used")), "assignment alone remains unused");
  }

  private static void readMarksLocalUsed() {
    var read = new Resolving.Variable("a", 2);
    var resolver = new Resolving.Resolver();
    resolver.resolve(new Resolving.Block(List.of(
        new Resolving.Var("a", new Resolving.Literal(1), 1),
        new Resolving.Expression(read))));
    check(resolver.errors().isEmpty(), "read local is not reported unused");
    check(resolver.binding(read).equals(new Resolving.Binding(0, 0)), "read receives depth and slot");
  }

  private static void ownInitializerIsRejected() {
    var resolver = new Resolving.Resolver();
    resolver.resolve(new Resolving.Block(List.of(
        new Resolving.Var("a", new Resolving.Variable("a", 7), 7))));
    check(resolver.errors().stream().anyMatch(e -> e.contains("own initializer")), "self initializer rejected");
  }

  private static void lexicalCoordinatesAndSlotsWork() {
    var outerRead = new Resolving.Variable("outer", 3);
    var innerRead = new Resolving.Variable("inner", 4);
    var resolver = new Resolving.Resolver();
    resolver.resolve(new Resolving.Block(List.of(
        new Resolving.Var("outer", new Resolving.Literal("O"), 1),
        new Resolving.Block(List.of(
            new Resolving.Var("inner", new Resolving.Literal("I"), 2),
            new Resolving.Expression(outerRead),
            new Resolving.Expression(innerRead))))));
    check(resolver.binding(outerRead).equals(new Resolving.Binding(1, 0)), "outer local has depth one, slot zero");
    check(resolver.binding(innerRead).equals(new Resolving.Binding(0, 0)), "inner local has depth zero, slot zero");

    var outer = new Resolving.SlotEnvironment(null, 1);
    outer.define(0, "O");
    var inner = new Resolving.SlotEnvironment(outer, 1);
    inner.define(0, "I");
    check(inner.getAt(1, 0).equals("O") && inner.getAt(0, 0).equals("I"), "array lookup follows coordinates");
    inner.assignAt(1, 0, "changed");
    check(outer.getAt(0, 0).equals("changed"), "array assignment follows coordinates");
  }

  private static void check(boolean condition, String message) {
    tests++;
    if (!condition) throw new AssertionError(message);
  }
}
