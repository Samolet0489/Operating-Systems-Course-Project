# IMCSH - IMC Shell

## Authors
- Samolet0489
- madarasibalazs

---

## Overview
The **IMCSH (IMC Shell)** is an alternative Linux shell implemented in C. It serves as a lightweight command-line interface capable of executing basic commands and managing processes. This shell interacts with the user via standard input and outputs results through standard output, implementing features like background process management and redirection.

---

## Features
1. **Execute Commands:**
   - `exec <program> [args]` - Executes a program with optional arguments.
   - Example: `exec ps -uax`

2. **Background Execution:**
   - Add `&` at the end of the `exec` command to run the process in the background.
   - Example: `exec sleep 5 &`

3. **Global Usage Information:**
   - `globalusage` - Displays version information.
   - Example output: `IMCSH Version 1.1 created by Samolet0489 and madarasibalazs`

4. **Output Redirection:**
   - Use `>` to redirect output to a file.
   - Example: `exec ls -l > directory_output.txt`

5. **Quit Command:**
   - `quit` - Exits the shell. If background processes are active, it prompts the user to confirm.

---

## External Libraries and Dependencies
The shell implementation uses standard C libraries:
- `<stdio.h>`: For input/output operations.
- `<stdlib.h>`: For memory allocation and process management.
- `<unistd.h>`: For process creation and file redirection.
- `<string.h>`: For string parsing.
- `<sys/types.h>` and `<sys/wait.h>`: For process control.
- `<fcntl.h>`: For file handling.
- `<signal.h>`: For managing signal handlers.

---

## Architecture and Key Components
### Main Modules
1. **Main Program:**
   - Implements the primary event loop to process user commands.
   - Displays a user-friendly prompt (`user@host>`).

2. **Command Parsing:**
   - **`parse_input`**: Tokenizes the user input into commands and arguments.

3. **Command Handlers:**
   - **`handle_exec`**: Manages command execution, background processing, and redirection.
   - **`handle_globalusage`**: Displays the shell version.
   - **`handle_quit`**: Handles the quit command, ensuring running background processes are safely managed.

4. **Signal Handling:**
   - **`sigchld_handler`**: Tracks background processes, removing completed ones.

---

## Execution Flow
1. **Command Prompt:**
   - The shell displays a prompt based on the current user and host.

2. **Command Parsing and Execution:**
   - Commands are tokenized and routed to the appropriate handler.

3. **Child Process Management:**
   - For `exec` commands, the shell forks a child process to execute the command.
   - Background processes are tracked in an internal array.

4. **Redirection and Background Execution:**
   - Redirection is managed using `dup2` for output files.
   - Background processes allow the shell to continue interacting with the user without waiting for command completion.

---

## Module Dependencies
### Process Flow:
- **Command Input** → **Parsing** → **Execution** → **Process/Redirection Management**

### Interactions:
- `exec` generates a child process.
- Redirection alters the standard output to a file descriptor.
- Background processes are tracked and managed through signal handling.

---

## How to Run
1. **Compiling:**
   - Use the provided `Makefile` to build the project:
     ```
     make
     ```

2. **Executing the Shell:**
   - Run the shell executable:
     ```
     ./imcsh
     ```

3. **Using the Shell:**
   - Enter commands following the syntax described in the features section.

4. **Exiting the Shell:**
   - Use the `quit` command to exit. Confirm if background processes are running.

---

## Limitations
- The shell does not support advanced piping or multi-command chaining (e.g., `|` or `&&`).
- Error handling for some edge cases (e.g., invalid commands) could be improved.

---

## Conclusion
The IMCSH provides a functional implementation of a Linux shell with features like command execution, background processing, output redirection, and version display. The modular design allows for straightforward extensibility. Future improvements could include support for piping, better error management, and additional built-in commands.

---

## Acknowledgements
This project was developed as part of the **Operating Systems** course (WS 2024/25) at IMC University.
