// kompilowanie kodu:   gcc procesy.c -o procesy
// uruchomienie programu:   ./procesy 5


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int fibonacci(int fibNum) {
    if (fibNum <= 1) {
        return fibNum;
    }
    else {
        return fibonacci(fibNum - 1) + fibonacci(fibNum - 2);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Error:\nProgram wymaga dok³adnie jednego argumentu z zakresu 1-13.\n");
        return 1;
    }

    int fibNum = atoi(argv[1]);

    if (fibNum <= 0) {
        fprintf(stderr, "Error:\nArgument musi byæ liczb¹ naaln¹ wiêksz¹ od zera.\n");
        return 2;
    }

    if (fibNum < 1 || fibNum > 13) {
        fprintf(stderr, "Error:\nArgument musi byæ liczb¹ z zakresu od 1 do 13.\n");
        return 3;
    }

    if (fibNum == 1 || fibNum == 2) {
        return 1;
    }

    pid_t child_1, child_2;
    int pid_stat_1, pid_stat_2;

    child_1 = fork();

    if (child_1 == 0) {
        char arg[20];
        snprintf(arg, sizeof(arg), "%d", fibNum - 1);

        execl(argv[0], argv[0], arg, NULL);
        fprintf(stderr, "Error funkcji execl dla dziecka 1");
        exit(1);
    }
    else if (child_1 < 0) {
        fprintf(stderr, "Error funkcji fork dla dziecka 1");
        exit(1);
    }
    else {
        waitpid(child_1, &pid_stat_1, 0);
    }

    child_2 = fork();

    if (child_2 == 0) {
        char arg[20];
        snprintf(arg, sizeof(arg), "%d", fibNum - 2);

        execl(argv[0], argv[0], arg, NULL);
        fprintf(stderr, "Error funkcji execl dla dziecka 2");
        exit(1);
    }
    else if (child_2 < 0) {
        fprintf(stderr, "Error funkcji fork dla dziecka 2");
        exit(1);
    }
    else {
        waitpid(child_2, &pid_stat_2, 0);
    } 
   
    pid_stat_1 >>= 8;
    pid_stat_2 >>= 8;

    printf("%d %d %s %d\n", getpid(), child_1, argv[1], pid_stat_1);
    printf("%d %d %s %d\n", getpid(), child_2, argv[1], pid_stat_2);
    printf("%d %d\n", getpid(), pid_stat_1 + pid_stat_2);

    return pid_stat_1 + pid_stat_2;
}