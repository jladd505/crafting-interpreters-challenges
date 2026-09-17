# Crafting Interpreters: Challenges

This repository contains the runnable code submitted with the written challenge
responses for Robert Nystrom's *Crafting Interpreters*.

## Build and test

## Notes

- The C list owns a separate heap copy of every inserted string.
- Deleting a node repairs both neighboring links and handles head/tail updates.
- `list_clear()` releases every string and node, leaving an empty reusable list.
- The Lox error cases are split into separate files so one expected failure does
  not prevent the successful cases from running.
