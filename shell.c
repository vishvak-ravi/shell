// PID: 730575776
// I pledge the COMP211 honor code.

// ----------------------------------------------
// These are the only libraries that can be
// used. Under no circumstances can additional
// libraries be included

#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

const char *PATH_SEPARATOR = ":";

// --------------------------------------------
// Currently only two builtin commands for this
// --------------------------------------------
const char* BUILT_IN_COMMANDS[] = { "cd", "exit", NULL };


/* ------------------------------------------------------------------------------
 *
 * Allocate memory for the command. 
 *
 * The length of `p_cmd->argv` is `argc+1`: the first `argc` slots are
 * used to store the arguments and the last one is set to NULL.
 *
 * Arguments:
 *      p_cmd : pointer to the command need to allocate memory.
 *      argc :  the number of arguments.
 *
 * Return:
 *      None
 */
void alloc_mem_for_command(command_t* p_cmd, int argc) {
    p_cmd->argc = argc;
    p_cmd->argv = (char**) malloc((1+argc)* sizeof(char*));
    for (int i = 0; i < p_cmd->argc; i++) {
        p_cmd->argv[i] = (char*) malloc(sizeof(char) * MAX_ARG_LEN);
    }
    p_cmd->argv[argc] = NULL;
} // end alloc_mem_for_command function

/* ------------------------------------------------------------------------------
 *
 * This function is used to free memory that may be malloc'd for the name and
 * argv fields in the command_t structure.
 *
 * Arguments:
 *         p_cmd : pointer to a command_t structure
 * Return: 
 *        None
 *
 */
void cleanup(command_t* p_cmd) {
    for (int i = 0; i < p_cmd->argc + 1; i++) {
        free(p_cmd->argv[i]);
    }

    free(p_cmd->argv);
    p_cmd->argc = 0;
    p_cmd = NULL;
}
/* ------------------------------------------------------------------------------
 *
 * This function parses the command and its arguments
 * specified by the user.
 *
 *  Arguments:
 *      line: pointer to the string containing the cmd.
 *      p_cmd: pointer to the command_t structure
 *
 *  Return:
 *      N/A
 *
 */
void parse(char* line, command_t* p_cmd) {
    char lineCopy[strlen(line) + 1];
    strcpy(lineCopy, line);

    char* argcCount = strtok(lineCopy, " ");

    //count argc
    p_cmd->argc = 0;
    while (argcCount != NULL) {
        p_cmd->argc++;
        argcCount = strtok(NULL, " ");
    }

    char *token = strtok(line, " ");
    alloc_mem_for_command(p_cmd, p_cmd->argc);

    //populate argv
    int i = 0;
    while (token != NULL) {
        strcpy(p_cmd->argv[i], token);
        token = strtok(NULL, " ");
        i++;
    }
} // end parse function

/* ------------------------------------------------------------------------------
 *
 * This function is determines if the named command entered
 * by the user in the shell can be found in one of the folders defined in the
 * PATH environment. If the file exists, then the name
 * of the executable at argv[0] is replaced with the location (fully qualified
 * path) of the executable.
 *
 *  Arguments:
 *         p_cmd: pointer to the command_t structure
 *  Return:
 *      true: if cmd is in the PATH
 *      false:  if not in the PATH.
 *
 */
bool find_fullpath(command_t* p_cmd) {
    struct stat buff;

    //copy path to prevent modification
    char *envPath = getenv("PATH");
    char *path = (char*) malloc(strlen(envPath) + 1 * sizeof(char));
    strcpy(path, envPath);

    char *token = strtok(path, PATH_SEPARATOR);
    char *temp = (char*) malloc(MAX_ARG_LEN * sizeof(char));

    //find path
    while (token != NULL) {
        memset(temp, 0, MAX_ARG_LEN);
        strcpy(temp, token);
        strcat(temp, "/");
        strcat(temp, p_cmd->argv[0]);
        if (stat(temp, &buff) == 0 && (S_IFREG & buff.st_mode)) {
            strcpy(p_cmd->argv[0], temp);
            return true;
        }
        token = strtok(NULL, PATH_SEPARATOR);
    }
    return false;
} // end find_fullpath function

/* ------------------------------------------------------------------------------
 *
 * This function will execute external commands.
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structor
 *
 * Return:
 *      SUCCESS: successfully execute the command.
 *      ERROR: error occurred.
 *
 */
int execute(command_t* p_cmd) {
    int status = SUCCESS;
    int child_process_status;
    pid_t child_pid;

    //run built-in program
    if (is_builtin(p_cmd)) {
        do_builtin(p_cmd);
        return status;
    }
    else {
        //run if path found
        if (find_fullpath(p_cmd)) {
            child_pid = fork();

            if(child_pid < 0) {
                return ERROR;
            }
            else if (child_pid == 0) {
                execv(p_cmd->argv[0], p_cmd->argv);
                exit(SUCCESS);
            }
            //parent waits when no error
            else {
                wait(&child_process_status);
            }
            return child_process_status;
        }
        else {
            printf("Command '%s' not found!\n", p_cmd->argv[0]);
            return status;
        }
    }
    return status;
} // end execute function

/* ------------------------------------------------------------------------------
 *
 * This function will determine if command (cmd for short) entered in the shell
 * by the user is a valid builtin command.
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      true:  the cmd is in array `valid_builtin_commands`.
 *      false: not in array `valid_builtin_commands`.
 *
 */
bool is_builtin(command_t* p_cmd) {
    int cnt = 0;

    while(BUILT_IN_COMMANDS[cnt] != NULL) {
        if (strcmp(p_cmd->argv[0], BUILT_IN_COMMANDS[cnt]) == 0) {
            return true;
        }

        cnt++;
    }

    return false;
} // end is_builtin function

/* ------------------------------------------------------------------------------
 *
 * This function is used execute built-in commands such as change directory (cd)
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      SUCCESS: no error occurred during chdir operation.
 *      ERROR: an error occured during chdir operation.
 *
 */
int do_builtin(command_t* p_cmd) {
    // If the command was 'cd', then change directories
    // otherwise, tell the program to EXIT

    struct stat buff;
    int status = SUCCESS;

    // exit
    if (strcmp( p_cmd->argv[0], "exit") == 0) {
        exit(status);
    }

    // cd
    if (p_cmd->argc == 1) {
        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental
        // variable

        status = chdir(getenv("HOME"));
    } else if ( (status = stat(p_cmd->argv[1], &buff)) == 0 && (S_IFDIR & buff.st_mode) ) {
        // -----------------------
        // cd with one arg
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir(p_cmd->argv[1]);
    }

    return status;

} // end do_builtin function
