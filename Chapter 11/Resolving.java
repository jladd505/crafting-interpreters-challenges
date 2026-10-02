package chapter11;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.IdentityHashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/** Resolver state tracking and array-indexed environments for Chapter 11. */
public final class Resolving {
  private Resolving() {}

  public sealed interface Expr permits Variable, Assign, Literal {}
  public record Variable(String name, int line) implements Expr {}
  public record Assign(String name, Expr value, int line) implements Expr {}
  public record Literal(Object value) implements Expr {}

  public sealed interface Stmt permits Var, Expression, Block {}
  public record Var(String name, Expr initializer, int line) implements Stmt {}
  public record Expression(Expr expression) implements Stmt {}
  public record Block(List<Stmt> statements) implements Stmt {}

  public enum State { DECLARED, DEFINED, READ }
  public record Local(String name, int line, int index, State state) {
    Local withState(State newState) { return new Local(name, line, index, newState); }
  }
  public record Binding(int depth, int index) {}

  public static final class Resolver {
    private final Deque<LinkedHashMap<String, Local>> scopes = new ArrayDeque<>();
    private final IdentityHashMap<Expr, Binding> bindings = new IdentityHashMap<>();
    private final List<String> errors = new ArrayList<>();

    public void beginScope() { scopes.push(new LinkedHashMap<>()); }
    public void endScope() {
      var scope = scopes.pop();
      for (Local local : scope.values()) {
        if (local.state() == State.DEFINED) {
          errors.add("[line " + local.line() + "] Local variable '" + local.name() + "' is not used.");
        }
      }
    }

    public void resolve(Stmt statement) {
      if (statement instanceof Var var) {
        declare(var.name(), var.line());
        if (var.initializer() != null) resolve(var.initializer());
        define(var.name());
      } else if (statement instanceof Expression expression) {
        resolve(expression.expression());
      } else if (statement instanceof Block block) {
        beginScope();
        for (Stmt child : block.statements()) resolve(child);
        endScope();
      }
    }

    public void resolve(Expr expression) {
      if (expression instanceof Variable variable) {
        if (!scopes.isEmpty()) {
          Local local = scopes.peek().get(variable.name());
          if (local != null && local.state() == State.DECLARED) {
            errors.add("[line " + variable.line() + "] Cannot read local variable in its own initializer.");
          }
        }
        resolveLocal(expression, variable.name(), true);
      } else if (expression instanceof Assign assign) {
        resolve(assign.value());
        resolveLocal(expression, assign.name(), false);
      }
    }

    private void declare(String name, int line) {
      if (scopes.isEmpty()) return;
      var scope = scopes.peek();
      if (scope.containsKey(name)) errors.add("[line " + line + "] Variable already declared in this scope.");
      scope.put(name, new Local(name, line, scope.size(), State.DECLARED));
    }

    private void define(String name) {
      if (scopes.isEmpty()) return;
      Local local = scopes.peek().get(name);
      scopes.peek().put(name, local.withState(State.DEFINED));
    }

    private void resolveLocal(Expr expression, String name, boolean isRead) {
      int depth = 0;
      for (var scope : scopes) {
        Local local = scope.get(name);
        if (local != null) {
          bindings.put(expression, new Binding(depth, local.index()));
          if (isRead && local.state() != State.DECLARED) scope.put(name, local.withState(State.READ));
          return;
        }
        depth++;
      }
    }

    public Binding binding(Expr expression) { return bindings.get(expression); }
    public List<String> errors() { return List.copyOf(errors); }
  }

  public static final class SlotEnvironment {
    private final SlotEnvironment enclosing;
    private final Object[] values;
    public SlotEnvironment(SlotEnvironment enclosing, int localCount) {
      this.enclosing = enclosing;
      this.values = new Object[localCount];
    }
    public void define(int index, Object value) { values[index] = value; }
    public Object getAt(int depth, int index) { return ancestor(depth).values[index]; }
    public void assignAt(int depth, int index, Object value) { ancestor(depth).values[index] = value; }
    private SlotEnvironment ancestor(int depth) {
      SlotEnvironment environment = this;
      for (int i = 0; i < depth; i++) environment = environment.enclosing;
      if (environment == null) throw new IllegalArgumentException("Invalid lexical depth.");
      return environment;
    }
  }
}

