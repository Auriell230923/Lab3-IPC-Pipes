// C program to demonstrate two-way communication using fork() and pipes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd1[2];
    int fd2[2];

    char howard_str[] = "howard.edu";
    char bison_str[] = "gobison.org";

    char first_input[100];
    pid_t p;

    // Create first pipe: Parent -> Child
    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe Failed\n");
        return 1;
    }

    // Create second pipe: Child -> Parent
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed\n");
        return 1;
    }

    // Get first input
    printf("Enter first string: ");
    fflush(stdout);

    if (fgets(first_input, sizeof(first_input), stdin) == NULL)
    {
        fprintf(stderr, "Input failed\n");
        return 1;
    }

    first_input[strcspn(first_input, "\n")] = '\0';

    // Create child process
    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed\n");
        return 1;
    }

    // P1 - Parent Process
    else if (p > 0)
    {
        close(fd1[0]);
        close(fd2[1]);

        // Send first input to child
        write(fd1[1], first_input, strlen(first_input) + 1);
        close(fd1[1]);

        // Receive updated string from child
        char final_string[300];

        read(fd2[0], final_string, sizeof(final_string));
        close(fd2[0]);

        // Add gobison.org
        strcat(final_string, bison_str);

        // Final output
        printf("Output : %s\n", final_string);

        wait(NULL);
    }

    // P2 - Child Process
    else
    {
        close(fd1[1]);
        close(fd2[0]);

        char child_string[300];
        char second_input[100];

        // Receive first string from parent
        read(fd1[0], child_string, sizeof(child_string));
        close(fd1[0]);

        // Add howard.edu
        strcat(child_string, howard_str);

        // Intermediate output
        printf("Output : %s\n", child_string);

        // Get second input
        printf("Enter second string: ");
        fflush(stdout);

        if (fgets(second_input, sizeof(second_input), stdin) == NULL)
        {
            fprintf(stderr, "Input failed\n");
            exit(1);
        }

        second_input[strcspn(second_input, "\n")] = '\0';

        // Add second input
        strcat(child_string, second_input);

        // Send updated string back to parent
        write(fd2[1], child_string, strlen(child_string) + 1);
        close(fd2[1]);

        exit(0);
    }

    return 0;
}