# myshell

A Unix shell implemented in C from scratch. Built to develop a strong
foundation in systems programming by working directly with processes,
file descriptors, and OS system calls — the same primitives that power
every shell you've ever used.

## Motivation

Most CS coursework teaches you to use the OS. This project teaches you
how it works. Implementing a shell requires you to understand fork/exec,
signal handling, pipes, and file descriptor manipulation at a level that
no textbook exercise can replicate.

Built as part of a self-directed path into low-level and embedded
software engineering.

## Build and run

Dependencies: gcc, make, valgrind (optional)

make          # compile
./myshell     # run
make clean    # remove build artifacts
make valgrind # run under valgrind for memory checking

## Architecture

| File | Responsibility |
|------|----------------|
| src/main.c | REPL loop — reads input until EOF, routes to parser and executor |
| src/parser.c | Tokenizes raw input string into an argv array using strtok |
| src/executor.c | Forks a child process and execs the parsed command via execvp |
| src/builtins.c | Handles commands that must run in the shell process itself (cd, exit) |
| include/shell.h | Shared constants — MAX_INPUT, MAX_ARGS, PROMPT, SHELL_NAME |

## Design decisions

**Why are builtins separate from executor?**
Commands like `cd` and `exit` must run inside the shell process itself.
If forked into a child, `cd` would change the child's directory and exit —
the shell's working directory would be completely unaffected.

**Why split parser and executor?**
Separation of concerns — each module has one job and can be tested
independently. Changing how input is parsed never touches execution logic.

## Status

| Milestone | Description | Status |
|-----------|-------------|--------|
| M0 | Project structure, Makefile, REPL loop | Complete |
| M1 | Command execution — fork, exec, waitpid | In progress |
| M2 | I/O redirection | Planned |
| M3 | Pipes | Planned |
| M4 | Signals and job control | Planned |

## Author

Miguel Salvador — CS Junior, Portland State University
github.com/msalvad2