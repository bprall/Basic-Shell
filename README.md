# Rudimentary-Shell
**Simple Shell Command Interpreter**

## Overview

This project implements a basic command interpreter (shell) in C. It reads user commands, supports input and output redirection, and executes commands using `fork` and `execvp`. The shell also handles errors gracefully and provides a minimal interactive environment for running commands.

## Features

- Command execution using `fork` and `execvp`.
- Input redirection (`<`).
- Output redirection (`>`).
- Output append redirection (`>>`).
- Simple error handling for common issues.
- Continuously prompts for user input until `exit` is entered.

## Prerequisites

- C compiler (e.g., `gcc`)
- POSIX-compatible operating system

## Compilation

To compile the shell, use the following command:

```sh
gcc -o simple_shell simple_shell.c
```

## Usage

Run the compiled shell executable:
```sh
./simple_shell
```

The shell will display a prompt (`cssh$`) where you can enter commands.

## Examples

- **Run a command:**
```sh
cssh$ ls -l
```

- **Input redirection:**
```sh
cssh$ cat < input.txt
```

- **Output redirection:**
```sh
cssh$ echo "Hello, World!" > output.txt
```

- **Append output redirection:**
```sh
cssh$ echo "Appending text" >> output.txt
```

- **Exit the shell:**
```sh
cssh$ exit
```

## Error Handling

- If multiple redirection operators are used incorrectly, the shell will print an error message.
- Common errors include:
    - Two or more input redirection operators (`<`).
    - Multiple output redirection operators (`>` or `>>`).
    - File access issues during redirection.
