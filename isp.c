#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
const char PIPE_CHAR = '|';
#define FALSE 0
#define TRUE 1
#define READ 0 // The index of the read end of the unnamed pipe
#define WRITE 1 // The index of the write end of the unnamed pipe
#define MAX_PIPE_COUNT 100 // The maximun number of normal pipes can be handled
int fd[MAX_PIPE_COUNT][2]; // Stores the file descriptors of all unnamed pipes opened
int fd2[MAX_PIPE_COUNT][2]; // Stores the file descriptors of all unnamed pipes opened

// Prototypes
void tokenizeString(char args[500], int *argc, char *argv[]);
void parseInput(int argc, char *argv[], char cmds[][50], int *cmdsc);
void runChildProcess(char *cmd, int pipeRead, int pipeWrite, int readFd[2], int writeFd[2]);

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

        // Using the normal pipe
        for (int i = 0; i < cmdsc; i++){
            if (cmdsc == 1){ // There is only one process, no pipe is needed
                runChildProcess(cmds[i], FALSE, FALSE, NULL, NULL);
            } 
            else { // Some pipes are needed
                if (i == (cmdsc - 1)){ // The last child will only read
                    runChildProcess(cmds[i], TRUE, FALSE, fd[i-1], NULL);
                    
                    close(fd[i-1][0]);
                    close(fd[i-1][1]);
                } 
                else {
                    // Need to create a pipe to write to
                    pipe (fd[i]);

                    if (i == 0){ // Will only write to next pipe
                        runChildProcess(cmds[i], FALSE, TRUE, NULL, fd[i]);
                    } 
                    else { // Will read from previous pipe and write to next pipe
                        runChildProcess(cmds[i], TRUE, TRUE, fd[i-1], fd[i]);

                        close(fd[i-1][0]);
                        close(fd[i-1][1]);
                    }
                }
            }
        }
        
        for (int i = 0; i < cmdsc; i ++){
            wait(NULL);
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

/**
 * Executes a command in string format
 * @param cmd The command in string format
 */
void runChildProcess(char *cmd, int pipeRead, int pipeWrite, int readFd[2], int writeFd[2]){
    pid_t pid;
    int status;

    pid = fork ();

    if (pid < 0){
        fprintf(stderr, "Fork kailed\n");
        return;
    }

    if (pid == 0){
        /*
            Handling pipe initialization
        */
        if (pipeRead){
            close (readFd[WRITE]); /* Close unused end*/ 
            dup2(readFd[READ], STDIN_FILENO);
        }

        if (pipeWrite){
            close (writeFd[READ]); /* Close unused end*/
            dup2(writeFd[WRITE], STDOUT_FILENO);
        }

        /*
            Execute the wanted command
        */
        int argc = 0;
        char *argv[100];
        tokenizeString(cmd, &argc, argv);
        if (execvp(argv[0], argv)){
            fprintf(stderr, "Failed executing command!\n\n");
            exit(1);
        }
    }
}