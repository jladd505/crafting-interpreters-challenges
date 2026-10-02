package chapter10;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/** Focused implementations for the Chapter 10 challenges. */
public final class Functions {
  private Functions() {}

  public record Selector(String text, int arity) {
    public static Selector smalltalk(String text) {
      int arity = 0;
      for (int i = 0; i < text.length(); i++) if (text.charAt(i) == ':') arity++;
      return new Selector(text, arity);
    }
  }

  public record FunctionExpr(List<String> parameters, List<String> body) {}
  public record FunctionStmt(String name, FunctionExpr function) {}

  public sealed interface Parsed permits Named, Anonymous {}
  public record Named(FunctionStmt declaration) implements Parsed {}
  public record Anonymous(FunctionExpr expression) implements Parsed {}

  /** A small parser showing the declaration/expression ambiguity and its resolution. */
  public static final class Parser {
    private final List<String> tokens;
    private int current;

    public Parser(String source) { tokens = tokenize(source); }

    public Parsed parse() {
      consume("fun");
      if (peekIdentifier() && peekNext("(")) {
        String name = advance();
        return new Named(new FunctionStmt(name, functionBody()));
      }
      return new Anonymous(functionBody());
    }

    private FunctionExpr functionBody() {
      consume("(");
      List<String> parameters = new ArrayList<>();
      if (!check(")")) {
        do { parameters.add(identifier()); } while (match(","));
      }
      consume(")");
      consume("{");
      List<String> body = new ArrayList<>();
      while (!check("}")) body.add(advance());
      consume("}");
      if (current != tokens.size()) throw new IllegalArgumentException("Unexpected trailing input.");
      return new FunctionExpr(List.copyOf(parameters), List.copyOf(body));
    }

    private String identifier() {
      if (!peekIdentifier()) throw new IllegalArgumentException("Expected an identifier.");
      return advance();
    }
    private boolean peekIdentifier() {
      return current < tokens.size() && tokens.get(current).matches("[A-Za-z_][A-Za-z0-9_]*");
    }
    private boolean peekNext(String text) {
      return current + 1 < tokens.size() && tokens.get(current + 1).equals(text);
    }
    private boolean check(String text) { return current < tokens.size() && tokens.get(current).equals(text); }
    private boolean match(String text) { if (!check(text)) return false; current++; return true; }
    private String advance() { if (current >= tokens.size()) throw new IllegalArgumentException("Unexpected end."); return tokens.get(current++); }
    private void consume(String text) { if (!match(text)) throw new IllegalArgumentException("Expected '" + text + "'."); }

    private static List<String> tokenize(String source) {
      String spaced = source.replaceAll("([(){},;])", " $1 ").trim();
      return spaced.isEmpty() ? List.of() : List.of(spaced.split("\\s+"));
    }
  }

  public static final class Environment {
    private final Environment enclosing;
    private final Map<String, Object> values = new HashMap<>();
    public Environment() { this(null); }
    public Environment(Environment enclosing) { this.enclosing = enclosing; }
    public void define(String name, Object value) { values.put(name, value); }
    public Object get(String name) {
      if (values.containsKey(name)) return values.get(name);
      if (enclosing != null) return enclosing.get(name);
      throw new IllegalArgumentException("Undefined variable '" + name + "'.");
    }
  }

  public static final class LoxFunction {
    private final String name;
    private final FunctionExpr declaration;
    private final Environment closure;
    public LoxFunction(String name, FunctionExpr declaration, Environment closure) {
      this.name = name;
      this.declaration = declaration;
      this.closure = closure;
    }
    public int arity() { return declaration.parameters().size(); }
    public Object captured(String variable) { return closure.get(variable); }
    @Override public String toString() { return name == null ? "<fn>" : "<fn " + name + ">"; }
  }

  /** Parameters and body locals occupy the same scope in Lox. */
  public static void validateFunctionScope(List<String> parameters, List<String> bodyLocals) {
    Set<String> names = new HashSet<>();
    for (String parameter : parameters) {
      if (!names.add(parameter)) throw new IllegalArgumentException("Duplicate parameter '" + parameter + "'.");
    }
    for (String local : bodyLocals) {
      if (!names.add(local)) throw new IllegalArgumentException("Already declared in this scope: '" + local + "'.");
    }
  }
}

