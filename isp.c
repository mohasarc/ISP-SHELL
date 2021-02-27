#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/* CONSTANTS */
#define PIPE_CHAR '|'
#define FALSE 0
#define TRUE 1
#define READ 0 // The index of the read end of the unnamed pipe
#define WRITE 1 // The index of the write end of the unnamed pipe
#define MAX_PIPE_COUNT 100 // The maximun number of normal pipes can be handled

/* GLOBAL VARIABLES */
int FDS_ARRAY_1[MAX_PIPE_COUNT][2]; // Stores the file descriptors of all unnamed pipes opened
int FDS_ARRAY_2[MAX_PIPE_COUNT][2]; // Stores the file descriptors of all unnamed pipes opened
int pipec = 0;
int statistics_char_count = 0;
int statistics_read_count = 0;
int statistics_write_count = 0;

/* PROTOTYPES */
void tokenizeString(char str[500], int *argNo, char *args[]);
void parseInput(int argNo, char *args[], char cmds[][50], int *cmdsc);
void runChildProcess(char *cmd, int pipeRead, int pipeWrite, int readFd[2], int writeFd[2]);
void moveData(int fd[2], int fd2[2], int N);
void createPipes(int **fd1, int **fd2, int mode);

/* FUNCTION DEFINITIONS */
/**
 * Reads the input and tokenizes it into an array of strings
 * @param str The string of user input
 * @param argNo The number of arguments
 * @param args The arguments
 */
void tokenizeString(char str[500], int *argNo, char *args[]){
    // Tokenize the string into string array
    int i = 0;
    char *p = strtok (str, " ");

    while (p != NULL)
    {
        args[i++] = p;
        *argNo = i;
        p = strtok (NULL, " ");
    }
}

/**
 *  Parses the arguments into an array of executable commands
 * @param argNo The number of arguments
 * @param args The arguments
 * @param cmds The executable commands array
 * @param cmdsc The number of executable commands
 */
void parseInput(int argNo, char *args[], char cmds[][50], int *cmdsc){
    int cmdCount = 0; // The number of full commands

    // go through the input
    for (int i = 0; i < argNo; i++){
        if (args[i][0] == PIPE_CHAR){
            cmdCount += 1;
        } else {
            strcat(cmds[cmdCount], args[i]);
            strcat(cmds[cmdCount], " ");
        }

    }

    *cmdsc = cmdCount + 1;
}

/**
 * Executes a command in string format
 * @param cmd The command in string format
 * @param pipeRead Whether the child will read from a pipe or not
 * @param pipeWrite Whether the child will write to a pipe or not
 * @param readFd The file descriptor of the file to read from
 * @param writeFd The file descriptor of the file to write to
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
            close(readFd[READ]);
        }

        if (pipeWrite){
            close (writeFd[READ]); /* Close unused end*/
            dup2(writeFd[WRITE], STDOUT_FILENO);
            close(writeFd[WRITE]);
        }

        /*
            Execute the wanted command
        */
        int argc = 0;
        char *argv[100];
        tokenizeString(cmd, &argc, argv);
        if (execvp(argv[0], argv)){
            fprintf(stderr, "Failed executing command!\n");
            exit(1);
        }
    }
}

/**
 * @param fd The pipe to read from
 * @param fd2 The pipe to write to
 * @param N The number of bytes to transfer at a time
 */
void moveData(int fd[2], int fd2[2], int N){
    int bytesRead = 1;
    char buf[N]; /* Parent process’s message buffer */
    statistics_char_count = 0;
    statistics_read_count = 0;
    statistics_write_count = 0;

    while (bytesRead)
    {
        // read N bytes from fd
        if ((bytesRead = read ( fd[READ], buf, N)) == -1){
            fprintf(stderr, "Failed reading from pipe\n");
            // exit(1);
        } else {
            statistics_char_count += bytesRead;
            statistics_read_count += 1;
        }

        // write N bytes to fd2
        if (write (fd2[WRITE], buf, bytesRead) == - 1){
            fprintf(stderr, "Failed writing to pipe\n");
            // exit(1);
        } else {
            statistics_write_count += 1;
        }

        if (bytesRead == 0){
            close(fd2[WRITE]);
            close(fd[READ]);
        } 
    }
}

/**
 * @param fd1 Pointer to the file descriptor of the first pipe
 * @param fd2 Pointer to the file descriptor of the second pipe (or the first pipe in case mode == 1)
 * @param mode The mode -> 1: normal mode, 2: tapped mode 
 */
void createPipes(int **fd1, int **fd2, int mode){
    switch (mode)
    {
    case 2:
        pipe (FDS_ARRAY_1[pipec]);
        pipe (FDS_ARRAY_2[pipec]);
        *fd1 = FDS_ARRAY_1[pipec];
        *fd2 = FDS_ARRAY_2[pipec];
    break;
    
    case 1:
    default:
        pipe (FDS_ARRAY_1[pipec]);
        *fd1 = FDS_ARRAY_1[pipec];
        *fd2 = FDS_ARRAY_1[pipec];
    break;
    }

    pipec++;
}

/* THE MAIN CODE */
int main(int argc, char *argv[])
{  
    int N = 0, mode = 0;

    if (argc == 3) {
        N = atoi(argv[1]);
        mode = atoi(argv[2]);
    }

    while (1) {
        int argsNo = 0;
        int cmdsc = 0;
        char str[500];
        char *args[100] = {'\0'};
        char cmds[100][50] = {'\0'};
        int *fd1;
        int *fd2;
        int *fd1prev;
        int *fd2prev;

        printf("isp$ ");
        scanf(" %[^\n]", str);
        tokenizeString(str, &argsNo, args);
        parseInput(argsNo, args, cmds, &cmdsc);

        for (int i = 0; i < cmdsc; i++){
            if (cmdsc == 1){ // There is only one process, no pipe is needed
                runChildProcess(cmds[i], FALSE, FALSE, NULL, NULL);
            } 
            else { // Some pipes are needed
                if (i == (cmdsc - 1)){ // The last child will only read
                    runChildProcess(cmds[i], TRUE, FALSE, fd2prev, NULL);
                    close(fd2prev[READ]);

                    if (mode == 2)
                        moveData(fd1prev, fd2prev, N);
                }
                else {
                    // Need to create some pipes to write to / read from
                    createPipes(&fd1, &fd2, mode); // when mode = 1 --> fd1 = fd2

                    if (i == 0){ // Will only write to next pipe
                        runChildProcess(cmds[i], FALSE, TRUE, NULL, fd1);
                    } 
                    else { // Will read from previous pipe and write to next pipe
                        runChildProcess(cmds[i], TRUE, TRUE, fd2prev, fd1);
                        close(fd2prev[READ]);

                        if (mode == 2)
                            moveData(fd1prev, fd2prev, N);
                    }

                    fd1prev = fd1;
                    fd2prev = fd2;
                    close(fd1[WRITE]);
                }
            }
        }

        for (int i = 0; i < cmdsc; i ++){
            wait(NULL);
        }

        if (mode == 2 && cmdsc > 1){
            // Printing statistics
            printf("============ STATISTICS ============\n");
            printf("character-count: %d\n", statistics_char_count);
            printf("read-call-count: %d\n", statistics_read_count);
            printf("write-call-count: %d\n", statistics_write_count);
        }
    }
}