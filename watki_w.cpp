
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Struktura przechowująca dane przekazywane do wątków
struct ThreadData {
    int start;
    int end;
    double* globalResult;
    HANDLE mutex;
};

// Funkcja obliczająca wartość π za pomocą wzoru Leibniza
double CalculatePI(int n) {
    double result = 0.0;

    for (int i = 0; i < n; i++) {
        double licznik = (i % 2 == 0) ? 1.0 : -1.0;
        double mianownik = 2.0 * i + 1.0;
        result += licznik / mianownik;
    }
    return result * 4.0;
}

// Funkcja wykonywana przez wątki
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    struct ThreadData* data = (struct ThreadData*)lpParam;
    
    double result = 0.0;

// Wypisanie identyfikatora wątku, rozmiaru zakresu i pierwszego elementu
    printf("Thread #%lu size = %d first = %d\n", GetCurrentThreadId(), data->end - data->start + 1, data->start);

// Obliczenia w zakresie
    for (int i = data->start; i <= data->end; i++) {
        double licznik = (i % 2 == 0) ? 1.0 : -1.0;
        double mianownik = 2.0 * i + 1.0;
        result += licznik / mianownik;
    }

// Sekcja krytyczna - aktualizacja zmiennej globalnej
    WaitForSingleObject(data->mutex, INFINITE);
   
    *(data->globalResult) += result;

// Zwolnienie mutexa
    ReleaseMutex(data->mutex);

    printf("Thread #%lu sum = %.17f\n", GetCurrentThreadId(), result);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s <liczba_wyrazow> <liczba_watkow>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int w = atoi(argv[2]);

    if (n <= 1 || n >= 1000000001 || w <= 1 || w >= 101) {
        fprintf(stderr, "Niepoprawne argumenty. Sprawdz warunki dla n i w.\n");
        return 1;
    }

// Inicjalizacja globalnej zmiennej i mutexa
    double globalResult ;
    HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        fprintf(stderr, "Blad inicjalizacji mutexa\n");
        return 1;
    }

// Utworzenie tablicy wątków
    HANDLE* threads = (HANDLE*)malloc(w * sizeof(HANDLE));
    if (threads == NULL) {
        fprintf(stderr, "Blad alokacji pamieci na watki\n");
        return 1;
    }

// Utworzenie struktur danych dla każdego wątka
    struct ThreadData* threadData = (struct ThreadData*)malloc(w * sizeof(struct ThreadData));
    if (threadData == NULL) {
        fprintf(stderr, "Blad alokacji pamieci na dane watkow\n");
        return 1;
    }

// Rozdzielenie zakresu dla wątków
    int elementsPerThread = n / w;
    int remainingElements = n % w;
    int currentStart = 0;

// Utworzenie i uruchomienie wątków z pomiarem czasu
    clock_t start = clock();

    for (int i = 0; i < w; i++) {
        threadData[i].start = currentStart;
        threadData[i].end = currentStart + elementsPerThread - 1 + (i < remainingElements ? 1 : 0);
        threadData[i].globalResult = &globalResult;
        threadData[i].mutex = mutex;

// Utworzenie wątku
        threads[i] = CreateThread(NULL, 0, ThreadFunction, &threadData[i], 0, NULL);
        if (threads[i] == NULL) {
            fprintf(stderr, "Blad tworzenia watku\n");
            return 1;
        }
        currentStart = threadData[i].end + 1;
    }

// Oczekiwanie na zakończenie wątków
    WaitForMultipleObjects(w, threads, TRUE, INFINITE);

    clock_t end = clock();
    double timesUpThreads = ((double)(end - start)) / CLOCKS_PER_SEC;

// Utworzenie i uruchomienie liczenia PI z pomiarem czasu
    start = clock();

    double piWithoutThreads = CalculatePI(n);

    end = clock();
    double timesUpWithoutThreads = ((double)(end - start)) / CLOCKS_PER_SEC;

// Wypisanie wyników
    printf("with/ Threads: PI = %.17f time = %.5f sec.\n", globalResult * 4.0, timesUpThreads);
    printf("without/ Threads: PI = %.17f time = %.5f sec.\n", piWithoutThreads, timesUpWithoutThreads);

// Zwolnienie zasobów
    CloseHandle(mutex);
    free(threads);
    free(threadData);

    return 0;
}