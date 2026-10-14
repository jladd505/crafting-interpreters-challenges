package chapter13;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/** Trait composition, BETA-style inner dispatch, and arrays. */
public final class Inheritance {
  private Inheritance() {}

  @FunctionalInterface
  public interface TraitMethod { Object call(Object receiver); }

  public record Trait(String name, Map<String, TraitMethod> methods) {
    public Trait { methods = Map.copyOf(methods); }
  }

  /** Flattens traits and rejects every collision, including duplicates in the class body. */
  public static Map<String, TraitMethod> composeTraits(
      List<Trait> traits, Map<String, TraitMethod> ownMethods) {
    Map<String, TraitMethod> combined = new LinkedHashMap<>();
    for (Trait trait : traits) {
      for (var entry : trait.methods().entrySet()) {
        putWithoutCollision(combined, entry.getKey(), entry.getValue(), trait.name());
      }
    }
    for (var entry : ownMethods.entrySet()) {
      putWithoutCollision(combined, entry.getKey(), entry.getValue(), "class body");
    }
    return Map.copyOf(combined);
  }

  private static void putWithoutCollision(
      Map<String, TraitMethod> methods, String name, TraitMethod method, String source) {
    if (methods.containsKey(name)) {
      throw new IllegalArgumentException(
          "Method collision for '" + name + "' while applying " + source + ".");
    }
    methods.put(name, method);
  }

  @FunctionalInterface
  public interface BetaMethod {
    void call(BetaInstance receiver, Runnable inner);
  }

  public static final class BetaClass {
    private final String name;
    private final BetaClass superclass;
    private final Map<String, BetaMethod> methods;

    public BetaClass(String name, BetaClass superclass, Map<String, BetaMethod> methods) {
      this.name = name;
      this.superclass = superclass;
      this.methods = Map.copyOf(methods);
    }

    public BetaInstance instantiate() { return new BetaInstance(this); }

    public void invoke(BetaInstance receiver, String methodName) {
      List<BetaMethod> chain = new ArrayList<>();
      collectTopDown(methodName, chain);
      if (chain.isEmpty()) throw new IllegalArgumentException("Undefined method '" + methodName + "'.");
      invokeAt(receiver, chain, 0);
    }

    private void collectTopDown(String methodName, List<BetaMethod> chain) {
      if (superclass != null) superclass.collectTopDown(methodName, chain);
      BetaMethod method = methods.get(methodName);
      if (method != null) chain.add(method);
    }

    private static void invokeAt(BetaInstance receiver, List<BetaMethod> chain, int index) {
      if (index >= chain.size()) return;
      chain.get(index).call(receiver, () -> invokeAt(receiver, chain, index + 1));
    }

    @Override public String toString() { return name; }
  }

  public record BetaInstance(BetaClass klass) {
    public void call(String name) { klass.invoke(this, name); }
  }

  /** Minimal native array primitive proposed by the official answer. */
  public static final class LoxArray {
    private final Object[] elements;

    public LoxArray(int size) {
      if (size < 0) throw new IllegalArgumentException("Array size cannot be negative.");
      elements = new Object[size];
    }

    public int length() { return elements.length; }
    public Object get(int index) { return elements[checked(index)]; }
    public Object set(int index, Object value) { elements[checked(index)] = value; return value; }

    private int checked(int index) {
      if (index < 0 || index >= elements.length) {
        throw new IndexOutOfBoundsException("Array index " + index + " out of bounds.");
      }
      return index;
    }

    @Override public String toString() { return Arrays.toString(elements); }
  }
}

