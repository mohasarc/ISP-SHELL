#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
const char PIPE_CHAR = '|';

// Prototypes
void tokenizeString(char args[500], int *argc, char *argv[]);
void parseInput(int argc, char *argv[], char cmds[][50], int *cmdsc);
void runChildProcess(char *cmd);

// The code
int main()
{
    while (1){
        int argc = 0;
        int cmdsc = 0;
        char args[500];
        char *argv[100] = {'\0'};
        char cmds[100][50] = {'\0'};

        printf("isp$ ");
        scanf(" %[^\n]", args);
        tokenizeString(args, &argc, argv);
        parseInput(argc, argv, cmds, &cmdsc);

        for (int i = 0; i < cmdsc; i++){
            runChildProcess(cmds[i]);
        }
    }
}

/**
 * Reads the input and tokenizes it into an array of strings
 * @param args The string of user input
 * @param argc The number of arguments
 * @param argv The arguments
 */
void tokenizeString(char args[500], int *argc, char *argv[]){
    // Tokenize the string into string array
    int i = 0;
    char *p = strtok (args, " ");

    while (p != NULL)
    {
        argv[i++] = p;
        *argc = i;
        p = strtok (NULL, " ");
    }
}

/**
 *  Parses the arguments into an array of executable commands
 * @param argc The number of arguments
 * @param argv The arguments
 * @param cmds The executable commands array
 * @param cmdsc The number of executable commands
 */
void parseInput(int argc, char *argv[], char cmds[][50], int *cmdsc){
    int cmdCount = 0; // The number of full commands

    // go through the input
    for (int i = 0; i < argc; i++){
        if (argv[i][0] == PIPE_CHAR){
            cmdCount += 1;
        } else {
            strcat(cmds[cmdCount], argv[i]);
            strcat(cmds[cmdCount], " ");
        }

    }

    *cmdsc = cmdCount + 1;
}

void runChildProcess(char *cmd){
    pid_t pid;
    int status;

    pid = fork ();

    if (pid < 0)
        return;

    if (pid == 0){
        // In the child process
        int argc = 0;
        char *argv[100] = {'\0'};
        tokenizeString(cmd, &argc, argv);
        execvp(argv[0], argv);
    } else {
        wait(&status);
    }
}