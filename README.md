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
| src/main.c | REPL loop — reads input, routes to parser and executor |
| src/parser.c | Splits input on | into pipeline_t, parses each segment into cmd_t with argv, infile, outfile using strtok_r |
| src/executor.c | Executes single commands via fork/exec/waitpid and pipelines via pipe/dup2 |
| src/builtins.c | Handles commands that must run in the shell process itself (cd, exit) |
| include/shell.h | Shared constants and structs — cmd_t, pipeline_t, MAX_INPUT, MAX_ARGS |

## Design decisions

**Why are builtins separate from executor?**
Commands like `cd` and `exit` must run inside the shell process itself.
If forked into a child, `cd` would change the child's directory and exit —
the shell's working directory would be completely unaffected.

**Why split parser and executor?**
Separation of concerns — each module has one job and can be tested
independently. Changing how input is parsed never touches execution logic.

## Features
- Command execution via fork/exec/waitpid
- I/O redirection — `>`, `>>`, `<`
- Command pipelines — `cmd1 | cmd2 | cmd3`
- Built-in commands — `cd`, `exit`, `jobs`
- Signal handling — Ctrl+C kills jobs, not the shell
- Job control — background jobs with &, jobs builtin
- Done notifications — background job completion reported at prompt
- Test suite — 17 tests across happy path, edge cases, error handling

## Status

| Milestone | Description                        | Status   |
|-----------|------------------------------------|----------|
| M0        | Project structure, Makefile, REPL  | Complete |
| M1        | Command execution — fork/exec/waitpid | Complete |
| M2        | I/O redirection                    | Complete |
| M3        | Pipes                              | Complete |
| M4        | Signals and job control            | Complete |
| M5        | Polish, testing, Valgrind          | Complete |

## What I learned
- How fork and exec are separate syscalls — and why that gap exists
  for fd manipulation before exec
- How dup2 rewires file descriptors transparently so programs never
  know about redirection
- How pipe reference counts control EOF delivery — and why every
  unused pipe end must be closed
- Why strtok is not reentrant and how strtok_r solves it
- Why builtins like cd must run in the shell process itself
- How to design a test suite covering all 11 test types across 
  happy path, edge cases, and error handling — and why testing 
  behavior not implementation makes tests resilient to change

## Author

Miguel Salvador — CS Junior, Portland State University
github.com/msalvad2
