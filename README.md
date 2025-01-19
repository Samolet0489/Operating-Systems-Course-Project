
# IMCSH: A Simple Linux Shell in C

## Description

IMCSH is a minimal shell written in C for educational purposes. It supports executing programs, background processes, output redirection, and an internal command to display shell usage info.

## Features

- **Prompt**: Displays `user@host>` where `user` is `$USER` and `host` is from `gethostname()`.
- **Foreground Execution**:
  - **Syntax**: `exec <program> [arguments]`
  - **Example**: `exec ps -aux`
  - Waits for the program to finish and prints PID afterward.
- **Background Execution**:
  - **Syntax**: `exec <program> [arguments] &`
  - **Example**: `exec ls -l &`
  - Does not wait. When it finishes, it prints its PID automatically.
- **globalusage**:
  - Prints `IMCSH Version 1.1 created by <author(s)>`.
  - Can be redirected using the `>` operator.
- **Output Redirection**:
  - **Syntax**: `[command] > filename`
  - Appends output to `filename`; creates it if it does not exist.
  - **Example**:
    ```bash
    exec ls -l > directory_output.txt
    ```
    Writes the output of `ls -l` to `directory_output.txt`.
    ```bash
    globalusage > usage.txt
    ```
    Writes the version info to `usage.txt`.
- **Quit**:
  - **Syntax**: `quit`
  - If background processes are running, the shell lists them and asks for confirmation.
  - If confirmed, kills all background processes and exits.

## How to Build

1. Clone or extract the source code to a directory.
2. Open a terminal in that directory.
3. Run `make` to compile:
   ```bash
   make
   ```
   This will produce an executable named `imcsh`.

## How to Use

1. Run the shell:
   ```bash
   ./imcsh
   ```
2. Type commands according to the syntax described:
   - **Foreground**: `exec <program> [arguments]`
   - **Background**: `exec <program> [arguments] &`
   - **Usage info**: `globalusage`
   - **Redirection**: `[command] > filename`
   - **Quit**: `quit`

### Example Session

```bash
youruser@yourhost> exec ls
file1.c   file2.txt
Process with PID 12345 finished
youruser@yourhost> exec ls -l > mylist.txt
Process with PID 12346 finished
youruser@yourhost> globalusage
IMCSH Version 1.1 created by <author(s)>
youruser@yourhost> exec sleep 10 &
youruser@yourhost> quit
The following processes are running, are you sure you want to quit? [Y/n]
  PID: 12347
```

If you type `Y`, the shell kills the background process and exits.

## Known Limitations

- Only basic whitespace tokenization (no quote handling, pipes, etc.).
- Minimal error checking on file permissions or invalid arguments.
- Simple array for background PIDs (max 256).

## Author(s)

- `<Author Name(s)>`
