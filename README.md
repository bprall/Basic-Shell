# Rudimentary-Shell
**Simple Shell Command Interpreter**

## Overview

This project implements a basic command interpreter (shell) in C. It reads user commands, supports input and output redirection, pipelining, and includes built-in commands and command history management. The shell handles signal processing for child process cleanup, and ensures proper memory management for command history and command arguments. It also utilizes `fork` and `execvp` for command execution and handles errors while providing a minimal interactive environment. The project includes a `Makefile` for easy compilation and adds support for file compression and decompression via `zip` and `unzip` commands.

## Features

- **Makefile:**
- **Command Execution:** Executes commands using `fork` and `execvp`.

- **Input Redirection:** Redirects input from a file using `<`.

- **Output Redirection:** Redirects output to a file using `>`.

- **Output Append Redirection:** Appends output to a file using `>>`.

- **Pipelining:** Chains commands using `|`.

- **Built-in Commands:**

  - `cd [dir]`: Change the current working directory.

  - `pwd`: Print the current working directory.

  - `history`: Show the history of commands entered.

- **File Compression and Decompression:**

  - `zip [input_file] [output_file]`: Compresses the input file to the specified output file.

  - `unzip [input_file] [output_file]`: Decompresses the input file to the specified output file.

- **Command History:** Maintains and displays a history of executed commands.

- **Signal Handling:** Properly handles signals, such as reaping zombie processes.

- **Error Handling:** Gracefully handles common errors related to redirection and command execution.

- **Interactive Prompt:** Continuously prompts for user input until `exit` is entered.

## Prerequisites

- C compiler (e.g., `gcc`)
- POSIX-compatible operating system

## Compilation

To compile the shell, use the following command:

```sh
make
```

## Usage

Run the compiled shell executable:
```sh
cd build
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
- **Command piping:**
```sh
cssh$ ls -l | grep "txt"
```

- **Change directory:**
```sh
cssh$ cd /path/to/directory
```

- **Print working directory:**
```sh
cssh$ pwd
```

- **View command history:**
```sh
cssh$ history
```

- **Zip a file:**
```sh
cssh$ zip example.txt example.zip
```

or with default output file name:
```sh
cssh$ zip example.txt
```

- **Unzip a file:**
```sh
cssh$ unzip example.zip example.txt
```

or with default output file name:
```sh
cssh$ unzip example.zip
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
- Errors in command execution and piping will be reported, with details provided by `perror`.