# Crafting Interpreters: Challenges

This repository contains the runnable code submitted with the written challenge
responses for Robert Nystrom's *Crafting Interpreters*.

## Contents

- `chapter01/java/Hello.java` - Java "Hello, world!"
- `chapter01/c/` - a doubly linked list of heap-allocated strings plus tests
- `chapter03/lox/` - Lox examples and focused edge-case programs
- `scripts/run_lox_tests.sh` - runs the Lox programs against the book's official
  Java interpreter source tree
- `Makefile` - builds and tests the Java and C work

## Build and test

Requirements: JDK 8 or newer, a C11 compiler, and `make`.

```sh
make test
```

To run the Lox programs, download the official book repository and pass its
root directory to the test script:

```sh
git clone https://github.com/munificent/craftinginterpreters.git
./scripts/run_lox_tests.sh ./craftinginterpreters
```

The Lox runner compiles `jlox` into a temporary directory, checks the output of
the successful programs, and confirms that the deliberate type and scope errors
produce the interpreter's documented nonzero exit statuses.

## Notes

- The C list owns a separate heap copy of every inserted string.
- Deleting a node repairs both neighboring links and handles head/tail updates.
- `list_clear()` releases every string and node, leaving an empty reusable list.
- The Lox error cases are split into separate files so one expected failure does
  not prevent the successful cases from running.