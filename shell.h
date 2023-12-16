#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdbool.h>

#define SUCCESS 0
#define ERROR -1
#define EXIT 1

#define DEBUG 0
#define MAX_ARG_LEN 100

typedef struct {

    int argc;    // total number of arguments
    char** argv; // argument values (must malloc)

} command_t;

/* ------------------------------------------------------------------------------
 *
 * Allocate memory for the command. The last element in `p_cmd->argv` is set
 * NULL.
 *
 * Arguments:
 *      p_cmd : pointer to the command need to allocate memory.
 *      argc :  the number of arguments.
 *
 * Return:
 *      None
 */
void alloc_mem_for_command(command_t* p_cmd, int argc);

/* ------------------------------------------------------------------------------
 *
 * This function is used to free memory that may be malloc'd for the name and
 * argv fields in the command_t structure.  To be safe, you should also set the
 * pointer values to NULL.
 *
 * HINT(s): See man page for more information about free function
 *
 * Arguments:
 *         p_cmd : pointer to a command_t structure
 * Return: 
 *        None
 *
 */
void cleanup(command_t* p_cmd);

/* ------------------------------------------------------------------------------
 *
 * This function will parse the command (cmd for short) and its arguments
 * specified by the user.
 *
 *  Arguments:
 *      line: pointer to the string contains the cmd.
 *      p_cmd: pointer to the command_t structure
 *
 *  Return:
 *      N/A
 *
 */
void parse(char* line, command_t* p_cmd);

/* ------------------------------------------------------------------------------
 *
 *
 * This function is used determine if the named command (cmd for short) entered
 * by the user in the shell can be found in one of the folders defined in the
 * PATH environment (env or short) variable. If the file exists, then the name
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
bool find_fullpath(command_t* p_cmd);

/* ------------------------------------------------------------------------------
 *
 * This function will execute commands (cmd for short).
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure.
 *
 * Return:
 *      SUCCESS: successfully execute the command.
 *      ERROR: error occurred.
 *
 */
int execute(command_t* p_cmd);

/* ------------------------------------------------------------------------------
 *
 * This function will determine if command (cmd for short) entered in the shell
 * by the user is a valid builtin command.
 *
 * HINT(s): Use valid_builtin_commands array defined in shell.c
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      true:  the cmd is in array `valid_builtin_commands`.
 *      false: not in array `valid_builtin_commands`.
 *
 */
bool is_builtin(command_t* p_cmd);

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
int do_builtin(command_t* p_cmd);

#endif // _SHELL_H_
