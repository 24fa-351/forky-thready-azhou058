#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void random_sleep(int pid) {
    srand(time(NULL) ^ pid);
    int sleep_time = rand() % 8 + 1;
    printf("and will sleep for %d seconds\n", sleep_time);
    sleep(sleep_time);
}

void create_processes_pattern1(int n) {
    printf("** Pattern 1: creating %d processes\n", n);
    pid_t pids[n]; // Array to store PIDs of children
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("Process %d (pid %d) created ", i, getpid());
            random_sleep(getpid());
            printf("Process %d (pid %d): exiting\n", i, getpid());
            exit(0);
        } else {
            printf("Parent: created child %d (pid %d)\n", i, pid);
            pids[i] = pid;
        }
    }
    
    printf("** Pattern 1: All processes created\n");

    // waits for all children to exit
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    printf("** Pattern 1: All children have exited\n");
}

void create_processes_pattern2(int n) {
    printf("** Pattern 2: creating %d processes\n", n);
    pid_t pid;
    pid_t pids[n]; // Array to store PIDs of children

    pid = fork();
    if (pid == 0) {
        printf("Child 0 (pid %d): starting\n", getpid());

        for (int i = 1; i < n; i++) {
            pid = fork();
            if (pid == 0) {
                printf("Child %d (pid %d) starting\n", i, getpid());

                // Only print the message for the last child
                if (i == n - 1) {
                    printf("Child %d (pid %d) [no children created] ", i, getpid());
                    random_sleep(pid);
                    printf("** Pattern 2: All children created\n");
                }
                exit(0);

            } else {
                pids[0] = getpid();
                printf("Child %d (pid %d) creating child %d (pid %d) ", i - 1, pids[i-1], i, pid);
                random_sleep(pid);
                pids[i] = pid;
                waitpid(pid, NULL, 0);
            }
        }
        exit(0);
        
    } else {
        printf("Parent: created child 0 (pid %d)\n", pid);
        pids[0] = pid;

        for (int i = n - 1; i >= 0; i--) {
            waitpid(pids[i], NULL, 0);
            printf("Child %d has exited\n", i); //fix issue with pids[i] not being the correct pid
        }
    }

    printf("** Pattern 2: All children have exited\n");
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <number of processes> <pattern>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int pattern = atoi(argv[2]);

    if (n < 1 || n > 256) {
        printf("Number of processes must be between 1 and 256.\n");
        return 1;
    }

    switch (pattern) {
        case 1:
            create_processes_pattern1(n);
            break;
        case 2:
            create_processes_pattern2(n);
            break;
        default:
            printf("Invalid pattern number. Choose 1 or 2\n");
            return 1;
    }

    return 0;
}
