#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pipe1[2];
    int pipe2[2];
    pid_t p2, p3;

    // Make sure the user entered one argument
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
        return 1;
    }

    // Create first pipe: cat -> grep
    if (pipe(pipe1) == -1)
    {
        perror("pipe");
        return 1;
    }

    // Create second pipe: grep -> sort
    if (pipe(pipe2) == -1)
    {
        perror("pipe");
        return 1;
    }

    // Create P2
    p2 = fork();

    if (p2 < 0)
    {
        perror("fork");
        return 1;
    }

    // P1 - Parent process
    if (p2 > 0)
    {
        // cat writes into pipe1
        dup2(pipe1[1], STDOUT_FILENO);

        // Close unused pipe ends
        close(pipe1[0]);
        close(pipe1[1]);

        close(pipe2[0]);
        close(pipe2[1]);

        // Execute: cat scores
        execlp("cat", "cat", "scores", NULL);

        // Only runs if execlp fails
        perror("execlp cat");
        exit(1);
    }

    // P2 - Child process
    else
    {
        // Create P3 (child's child)
        p3 = fork();

        if (p3 < 0)
        {
            perror("fork");
            exit(1);
        }

        // P2 - grep process
        if (p3 > 0)
        {
            // grep reads from pipe1
            dup2(pipe1[0], STDIN_FILENO);

            // grep writes into pipe2
            dup2(pipe2[1], STDOUT_FILENO);

            // Close original pipe descriptors
            close(pipe1[0]);
            close(pipe1[1]);

            close(pipe2[0]);
            close(pipe2[1]);

            // Execute: grep <argument>
            execlp("grep", "grep", argv[1], NULL);

            // Only runs if execlp fails
            perror("execlp grep");
            exit(1);
        }

        // P3 - Child's child process
        else
        {
            // sort reads from pipe2
            dup2(pipe2[0], STDIN_FILENO);

            // Close all original pipe descriptors
            close(pipe1[0]);
            close(pipe1[1]);

            close(pipe2[0]);
            close(pipe2[1]);

            // Execute: sort
            execlp("sort", "sort", NULL);

            // Only runs if execlp fails
            perror("execlp sort");
            exit(1);
        }
    }

    return 0;
}