#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>

/* Global definitions */
#define MAX_LINE_LEN 1024
#define MAX_TOKENS 128
#define MAX_BG_PROCS 256


/*
 * IMCSH Version 1.1 created by <author(s)>
 *
 * Features:
 * 1) exec <program> [args]
 * 2) exec <program> [args] &
 * 3) globalusage
 * 4) redirection with ">"
 * 5) quit (with prompt if background processes are running)
 *
 * Note: No usage of system() - direct execvp calls only.
 */

/* Store PIDs of background processes */
static pid_t bgProcs[MAX_BG_PROCS];
static int bgCount = 0;


/* Version info string */
const char *IMCSH_VERSION = "IMCSH Version 1.1 created by <author(s)>";


/* Forward declarations */
void parse_input(char *input, char **tokens, int *num_tokens);
void handle_exec(char **tokens, int num_tokens);
void handle_globalusage(char **tokens, int num_tokens);
void handle_quit(void);
void remove_bg_pid(pid_t pid);


/* 
 * SIGCHLD handler: reaps finished background processes 
 * and prints their PIDs. 
 */
void sigchld_handler(int sig)
{
    (void)sig;  // unused
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Printt PID of the finished background process
        printf("Process with PID %d finished\n", pid);
        remove_bg_pid(pid);
    }
}


/* Remove a PID from the array of background processes */
void remove_bg_pid(pid_t pid)
{
    for (int i = 0; i < bgCount; i++) {
        if (bgProcs[i] == pid) {
            /* Move the last PID in the list to this position */
            bgProcs[i] = bgProcs[bgCount - 1];
            bgCount--;
            break;
        }
    }
}



int main(void)
{
   /* Install SIGCHLD handler to track background processes */
    signal(SIGCHLD, sigchld_handler);

    char input_line[MAX_LINE_LEN];
    char *tokens[MAX_TOKENS];
    int num_tokens;

   /* Build prompt: user@host> */
    char *username = getenv("USER");
    if (!username) {
        username = "unknown";
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "unknown_host");
    }

    /* Main loop: read commands until 'quit' */
    while (1) {
        /* Print prompt: user@host> */
        printf("%s@%s> ", username, hostname);
        fflush(stdout);

        /* Read input from user */
        if (!fgets(input_line, MAX_LINE_LEN, stdin)) {
            /* If EOF or error, exit the loop */
            printf("\n");
            break;
        }

        /* Strip trailing newline if present */
        size_t len = strlen(input_line);
        if (len > 0 && input_line[len - 1] == '\n') {
            input_line[len - 1] = '\0';
        }

        /* Parse input into tokens */
        parse_input(input_line, tokens, &num_tokens);

        /* If nothing typed, just continue */
        if (num_tokens == 0) {
            continue;
        }

        /* Check the first token to decide action */
        if (strcmp(tokens[0], "quit") == 0) {
            handle_quit();
            /* if handle_quit returns, user said "No", so continue. 
               If user said "Yes", we would have exited. */
        }
        else if (strcmp(tokens[0], "exec") == 0) {
            handle_exec(tokens, num_tokens);
        }

        else if (strcmp(tokens[0], "exec") == 0){
            handle_exec(tokens, num_tokens);
        }
        else {
            fprintf(stderr, "Unknown command: %s\n", tokens[0]);
        }
    }

    return 0;
}


/*
 * Parse the input string into tokens separated by whitespace.
 * tokens array will be NULL-terminated.
 */

void parse_input(char *input, char **tokens, int *num_tokens)
{
    *num_tokens = 0;
    char *token = strtok(input, " \t");
    while (token != NULL && *num_tokens < MAX_TOKENS - 1) {
        tokens[(*num_tokens)++] = token;
        token = strtok(NULL, " \t");
    }
    tokens[*num_tokens] = NULL;
}



/*
 * Handle the "exec" command.
 * Usage:  exec <program> [args] [&] [ > filename ]
 */
void handle_exec(char **tokens, int num_tokens)
{
    // tokens[0] == "exec"
    // The program to be executed is at tokens[1], plus optional arguments.
    if (num_tokens < 2) {
        fprintf(stderr, "Usage: exec <program> [arguments] [&] [> filename]\n");
        return;
    }

    /* 
     * We must detect:
     * 1) If & is present at the end for background execution
     * 2) If > is present for redirection
     * 3) Program name and its arguments
     */

    int background = 0;
    char *filename = NULL;

    // We'll collect the program and its args in a separate array
    char *cmdArgs[MAX_TOKENS];
    int cmdCount = 0;

    for (int i = 1; i < num_tokens; i++) {
        /* Check if the last token is "&" => background */
        if (strcmp(tokens[i], "&") == 0 && i == num_tokens - 1) {
            background = 1;
            break;
        }
        /* Check if ">" is found => next token is filename (if any) */
        else if (strcmp(tokens[i], ">") == 0) {
            if (i + 1 < num_tokens) {
                filename = tokens[i + 1];
            } else {
                fprintf(stderr, "Error: '>' found but no file specified.\n");
            }
            break;
        }
        else {
            // Otherwise, it's part of the command
            cmdArgs[cmdCount++] = tokens[i];
        }
    }
    // Null-terminate the command array
    cmdArgs[cmdCount] = NULL;

    if (cmdCount == 0) {
        fprintf(stderr, "Error: No program specified after 'exec'.\n");
        return;
    }

    // Fork a child to run the program
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    else if (pid == 0) {
        // Child process

        // If we have a filename for redirection, set it up
        if (filename != NULL) {
            int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                close(fd);
                exit(EXIT_FAILURE);
            }
            close(fd);
        }

        // Execute the program
        execvp(cmdArgs[0], cmdArgs);
        // If execvp returns, there's an error
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else {
        // Parent process
        if (background) {
            // Non-blocking: add to background list
            if (bgCount < MAX_BG_PROCS) {
                bgProcs[bgCount++] = pid;
            }
        } else {
            // Wait for child to finish, then print its PID
            int status;
            waitpid(pid, &status, 0);
            printf("Process with PID %d finished\n", pid);
        }
    }
}

/*
 * Handle globalusage command:
 * "globalusage" or "globalusage > filename
 */
void handle_globalusage(char **tokens, int  num_tokens)
{
    //We look for ">" and a filename
    char * filename = NULL;
    for (int i = 1; i < num_tokens; i++){
        if(strcmp(tokens[i], ">") == 0 && i + 1 < num_tokens){
            filename = tokens[i + 1];
            break;
        }
    }

    if(!filename){
        //no redirection, just print
        printf("%s\n", IMCSH_VERSION);
    } else {
        // redirect outout to filename
        int fd = open(filename, O_WRONLY | O_CREAT | O_ACCMODE, 0666);
        if (fd < 0)
        {
            perror("open");
            return;
        }

        //save current stdout
        int saved_stdout = dup(STDOUT_FILENO);

        //redirect
        if (dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("dup2");
            close (fd);
            return;

        }
        close (fd);

        // print version
        printf("%s\n", IMCSH_VERSION);

        //Restore stdout
        fflush(stdout);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        
        
    }
}

/*
 * Handle the "quit" command.
 * If background processes are running, prompt to confirm.
 * If user says Y, kill them and quit. Otherwise continue.
 */
void handle_quit(void)
{
    if (bgCount == 0) {
        // No background processes, just quit
        exit(0);
    } else {
        // Prompt user
        printf("The following processes are running, are you sure you want to quit? [Y/n]\n");
        for (int i = 0; i < bgCount; i++) {
            printf("  PID: %d\n", bgProcs[i]);
        }

        // Read user input
        char answer[8];
        if (!fgets(answer, sizeof(answer), stdin)) {
            return; // treat as "no"
        }

        // check first character
        if (answer[0] == 'Y' || answer[0] == 'y') {
            // Kill them all
            for (int i = 0; i < bgCount; i++) {
                kill(bgProcs[i], SIGKILL);
            }
            exit(0);
        } else {
            // do nothing, return to main loop
            return;
        }
    }
}








