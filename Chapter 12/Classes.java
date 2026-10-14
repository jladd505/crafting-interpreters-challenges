package chapter12;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/** Focused runtime objects for the Chapter 12 challenges. */
public final class Classes {
  private Classes() {}

  @FunctionalInterface
  public interface Body {
    Object call(LoxInstance receiver, List<Object> arguments);
  }

  public record Method(String name, int arity, boolean getter, Body body) {
    public BoundMethod bind(LoxInstance receiver) {
      return new BoundMethod(this, receiver);
    }
  }

  public record BoundMethod(Method method, LoxInstance receiver) {
    public Object call(Object... arguments) {
      if (arguments.length != method.arity()) {
        throw new IllegalArgumentException(
            "Expected " + method.arity() + " arguments but got " + arguments.length + ".");
      }
      return method.body().call(receiver, List.of(arguments));
    }
  }

  public static class LoxInstance {
    private final LoxClass klass;
    private final Map<String, Object> fields = new HashMap<>();

    public LoxInstance(LoxClass klass) { this.klass = klass; }

    public Object get(String name) {
      if (fields.containsKey(name)) return fields.get(name);
      if (klass != null) {
        Method method = klass.findMethod(name);
        if (method != null) {
          BoundMethod bound = method.bind(this);
          return method.getter() ? bound.call() : bound;
        }
      }
      throw new IllegalArgumentException("Undefined property '" + name + "'.");
    }

    public void set(String name, Object value) { fields.put(name, value); }

    @Override public String toString() {
      return klass == null ? "instance" : klass.name() + " instance";
    }
  }

  /** A class is itself an instance; its class is its metaclass. */
  public static final class LoxClass extends LoxInstance {
    private final String name;
    private final Map<String, Method> methods;

    public LoxClass(LoxClass metaclass, String name, Map<String, Method> methods) {
      super(metaclass);
      this.name = name;
      this.methods = Map.copyOf(methods);
    }

    public String name() { return name; }
    public Method findMethod(String name) { return methods.get(name); }
    public LoxInstance instantiate() { return new LoxInstance(this); }
    @Override public String toString() { return name; }
  }

  /** Builds a regular class and the metaclass that holds its class methods. */
  public static LoxClass defineClass(
      String name, Map<String, Method> instanceMethods, Map<String, Method> classMethods) {
    LoxClass metaclass = new LoxClass(null, name + " metaclass", classMethods);
    return new LoxClass(metaclass, name, instanceMethods);
  }
}

