# Custom-Shell
**Custom Command Interpreter**

## Overview

This project implements a basic command interpreter (shell) in C, designed to read user commands and support input and output redirection, pipelining, sequential command execution with `&&`, and command history management. The shell manages signal processing for child process cleanup, ensures memory management for command history and arguments, and uses `fork` and `execvp` for executing commands while handling errors effectively. It includes a `Makefile` for easy compilation and supports file compression and decompression with `zip` and `unzip` commands. Additionally, the shell provides the `wc` command to count lines, words, and characters in files or standard input, and the `help` command to display usage information for specific commands, offering general help if no command is specified.

## Features

- **Makefile:** Includes a `Makefile` for conveniently compiling the shell.

- **Command Execution:** Executes commands using `fork` and `execvp`.

- **Input Redirection:** Redirects input from a file using `<`.

- **Output Redirection:** Redirects output to a file using `>`.

- **Output Append Redirection:** Appends output to a file using `>>`.

- **Pipelining:** Chains commands using `|`.

- **Sequential Command Execution:** Supports executing commands sequentially with `&&`, where each command is executed only if the previous command succeeds.

- **Built-in Commands:**

  - `alias [alias_name="command"]`: Define a new alias, update an existing alias with the specified command, or list existing aliases if no alias name and command are provided.

  - `cat [file...]`: Concatenate and display the content of files.

  - `cd [dir]`: Change the current working directory.

  - `clear`: Clear the terminal screen.

  - `echo [text]`: Print the specified text to the terminal.

  - `grep [pattern] [file...]`: Search for a specified pattern in files and print matching lines.

  - `help [command]`: Display usage information for a specific command. If no command is specified, show general help.

  - `history [n]`: Show full command history or the last **n** commands in the history.

  - `kill [pid]`: Terminate a process by its process ID (PID).

  - `mkdir [dir]`: Create a new directory.

  - `mv [source] [destination]`: Move or rename files and directories.

  - `pwd`: Print the current working directory.

  - `rm [file...]`: Remove files. Supports options for recursive removal and force deletion.

  - `rmdir [dir]`: Remove an empty directory.

  - `sort [-r]`: Sort a list of numbers in ascending by standard input. If -r is specified, it sorts the numbers in reverse order.

  - `sortwords [input.txt] [output.txt]`: Read words from an input file, sort them, and save them to an output file, or overwrite the input file if no output file is specified.

  - `unalias [alias_name]`: Remove an existing alias.

  - `wc [-l] [-w] [-c] [FILES...]`: Print the number of lines, words, and characters in files. If no files are given, it reads from standard input.

- **File Compression and Decompression:**

  - `zip [input_file] [output_file]`: Compresses the input file to the specified output file.

  - `unzip [input_file] [output_file]`: Decompresses the input file to the specified output file.

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
./main
```

The shell will display a prompt (`cssh$`) where you can enter commands.

## Command Examples

- **Change Directory and List Files:**
```sh
cssh$ cd /path/to/directory && ls -l
```

- **Print Working Directory and View Files:**
```sh
cssh$ pwd && ls -l
```

- **Redirect Input and Count Lines in a File:**
```sh
cssh$ wc -l < input.txt
```

- **Redirect Output and View Changes:**
```sh
cssh$ echo "Hello, World!" > output.txt && cat output.txt
```

- **Append Output and Sort the File Content:**
```sh
cssh$ echo "Appending text" >> output.txt && sortwords output.txt
```

- **Search for Patterns and Display Results:**
```sh
cssh$ grep "pattern" file.txt
```

- **View Command History and Search:**
```sh
cssh$ history | grep "search_term"
```

- **Zip and Unzip Files with Default Names:**
```sh
cssh$ zip example.txt example.zip && unzip example.zip
```

- **Define Alias and Execute:**
```sh
cssh$ alias list="ls" && list
```

- **Remove an Alias and Verify:**
```sh
cssh$ unalias list && history
```

## Error Handling

- If multiple redirection operators are used incorrectly, the shell will print an error message.
- Common errors include:
  - Two or more input redirection operators (`<`).
  - Multiple output redirection operators (`>` or `>>`).
  - File access issues during redirection.
- Errors in command execution and piping will be reported, with details provided by `perror`.
