package chapter12;

import java.util.Map;

public final class Chapter12Tests {
  private static int tests;

  public static void main(String[] args) {
    classMethodsUseMetaclassLookup();
    gettersRunOnPropertyAccess();
    ordinaryZeroArgumentMethodsRemainCallable();
    openFieldsStillWork();
    System.out.println("Chapter 12: " + tests + " tests passed.");
  }

  private static void classMethodsUseMetaclassLookup() {
    var square = new Classes.Method("square", 1, false,
        (receiver, arguments) -> {
          double n = (double) arguments.get(0);
          return n * n;
        });
    var math = Classes.defineClass("Math", Map.of(), Map.of("square", square));
    Object property = math.get("square");
    check(property instanceof Classes.BoundMethod, "class method binds through metaclass");
    check(((Classes.BoundMethod) property).call(3.0).equals(9.0), "Math.square computes 9");
  }

  private static void gettersRunOnPropertyAccess() {
    var area = new Classes.Method("area", 0, true,
        (receiver, arguments) -> {
          double radius = (double) receiver.get("radius");
          return Math.PI * radius * radius;
        });
    var circleClass = Classes.defineClass("Circle", Map.of("area", area), Map.of());
    var circle = circleClass.instantiate();
    circle.set("radius", 4.0);
    double value = (double) circle.get("area");
    check(Math.abs(value - Math.PI * 16.0) < 0.000001, "getter executes eagerly");
  }

  private static void ordinaryZeroArgumentMethodsRemainCallable() {
    var label = new Classes.Method("label", 0, false, (receiver, arguments) -> "circle");
    var klass = Classes.defineClass("Shape", Map.of("label", label), Map.of());
    Object property = klass.instantiate().get("label");
    check(property instanceof Classes.BoundMethod, "ordinary method lookup returns callable");
    check(((Classes.BoundMethod) property).call().equals("circle"), "ordinary method runs on call");
  }

  private static void openFieldsStillWork() {
    var instance = Classes.defineClass("Open", Map.of(), Map.of()).instantiate();
    instance.set("answer", 42.0);
    check(instance.get("answer").equals(42.0), "Lox-style open field access retained");
  }

  private static void check(boolean condition, String message) {
    tests++;
    if (!condition) throw new AssertionError(message);
  }
}

