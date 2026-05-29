#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    char input[1024];
    char *args[22]; //20 for arguments, 1 for command, 1 for NULL terminator

    while (1) {
        printf(">> ");
        fflush(stdout); //say, print right now

        if (fgets(input, sizeof(input), stdin) == NULL) break; //record data

        input[strcspn(input, "\n")] = 0; //change \n to \0

        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL && i < 21) { //break it down into pieces
            i++;
            args[i] = strtok(NULL, " "); 
        }
        args[i] = NULL;

        if (args[0] == NULL) continue; //if the user just pressed enter, prompt again

        if (strcmp(args[0], "leave") == 0 && args[1] == NULL) break; //leave the shell

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
        } else if (pid == 0) { //--------child
            execv(args[0], args); //try to execute the command directly
            char *path = getenv("PATH"); //get the PATH environment variable

            if (path != NULL) { //if PATH is set, try to find the command in the directories listed in PATH
                char path_copy[1024];
                strcpy(path_copy, path); //make a copy of PATH because strtok modifies the string
                char *dir = strtok(path_copy, ":"); //split PATH into directories

                while (dir != NULL) { //for each directory in PATH, construct the full path to the command and try to execute it
                    char full_path[1024];
                    snprintf(full_path, sizeof(full_path), "%s/%s", dir, args[0]); //construct the full path to the command
                    execv(full_path, args); //try to execute the command with the full path
                    dir = strtok(NULL, ":"); //move to the next directory in PATH
                }

            }
            printf("command not found\n");
            exit(1);

        } else wait(NULL); //--------parent
    }
    return 0;
}