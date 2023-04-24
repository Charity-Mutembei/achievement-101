#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

#define MAX_CMD_LEN 1024
/**
 * prompt - Displays the prompt symbol to indicate
 * that the shell is ready to receive input.
*/
void prompt(void)
{
printf("$ ");
}
/**
 * read_command - Reads a command from the user's input using fgets() function
 * and returns it as a string. If fgets() returns NULL,
 * it means that the end of the input has been reached
 *  and the program should exit.
 *
 *  Return: The command string entered by the user.
*/
char *read_command(void)
{
char cmd[MAX_CMD_LEN];
if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL)
{
printf("\n");
exit(0);
}
cmd[strcspn(cmd, "\n")] = '\0';
return (strdup(cmd));
}
/**
 * parse_command - Takes a command string as input
 * and splits it into individual words
 * using the first word as the command and the
 * remaining words as its arguments.
 * @cmd: The command string to be parsed.
 * @args: An array to store the parsed words.
*/
void parse_command(char *cmd, char **args)
{
args[0] = cmd;
args[1] = NULL;
}
/**
 * execute_command - Execute a command.
 * @args: An array of arguments.
 *
 * Return: 1 if the shell should continue running, 0 if it should exit.
 */
void execute_command(char **args)
{
/* Fork child process to execute command*/
pid_t pid = fork();
if (pid == -1)
{
perror("fork");
exit(1);
}
else if (pid == 0)
{
/* Child process*/
if (execve(args[0], args, environ) == -1)
{
perror("./hsh");
exit(1);
}
}
else
{
/* Parent process*/
int status;
if (wait(&status) == -1)
{
perror("wait");
exit(1);
}
}
}
/**
 * main - Entry point of the simple shell
 *
 * Return: Always 0 on success
 */
int main(void)
{
char *cmd;
char *args[2];

while (1)
{
prompt();
cmd = read_command();
parse_command(cmd, args);
execute_command(args);
free(cmd);
}

return (0);
}
