#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

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
        fprintf(stderr, "Error:\nProgram requires exactly one argument in the range 1-13.\n");
        return 1;
    }

    int fibNum = atoi(argv[1]);

    if (fibNum <= 0) {
        fprintf(stderr, "Error:\nArgument must be a natural number greater than zero.\n");
        return 2;
    }

    if (fibNum < 1 || fibNum > 13) {
        fprintf(stderr, "Error:\nArgument must be a number in the range from 1 to 13.\n");
        return 3;
    }

    if (fibNum == 1 || fibNum == 2) {
        return 1;
    }

    STARTUPINFO si_1, si_2;
    PROCESS_INFORMATION pi_1, pi_2;

    ZeroMemory(&si_1, sizeof(si_1));
    si_1.cb = sizeof(si_1);
    ZeroMemory(&pi_1, sizeof(pi_1));

    ZeroMemory(&si_2, sizeof(si_2));
    si_2.cb = sizeof(si_2);
    ZeroMemory(&pi_2, sizeof(pi_2));

    char arg_1[20], arg_2[20];
    snprintf(arg_1, sizeof(arg_1), "%d", fibNum - 1);
    snprintf(arg_2, sizeof(arg_2), "%d", fibNum - 2);

    if (!CreateProcessA(NULL, argv[0], NULL, NULL, FALSE, 0, NULL, NULL, (LPSTARTUPINFOA)&si_1, &pi_1)) {
        fprintf(stderr, "Error creating child process 1\n");
        return 4;
    }

    if (!CreateProcessA(NULL, argv[0], NULL, NULL, FALSE, 0, NULL, NULL, (LPSTARTUPINFOA)&si_2, &pi_2)) {
        fprintf(stderr, "Error creating child process 2\n");
        return 5;
    }

    WaitForSingleObject(pi_1.hProcess, INFINITE);
    WaitForSingleObject(pi_2.hProcess, INFINITE);

    DWORD exitCode_1, exitCode_2;
    GetExitCodeProcess(pi_1.hProcess, &exitCode_1);
    GetExitCodeProcess(pi_2.hProcess, &exitCode_2);

    printf("%d %d %s %lu\n", GetCurrentProcessId(), pi_1.dwProcessId, argv[1], exitCode_1);
    printf("%d %d %s %lu\n", GetCurrentProcessId(), pi_2.dwProcessId, argv[1], exitCode_2);
    printf("%d %lu\n", GetCurrentProcessId(), exitCode_1 + exitCode_2);

    CloseHandle(pi_1.hProcess);
    CloseHandle(pi_1.hThread);
    CloseHandle(pi_2.hProcess);
    CloseHandle(pi_2.hThread);

    return exitCode_1 + exitCode_2;
}
