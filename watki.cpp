// SO IN1 21A LAB07
// PRZEMYS£AW TARKOWSKI
// tp54938@zut.edu.pl


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 100

// Struktura przechowuj¹ca dane przekazywane do w¹tków
typedef struct {
    int start;
    int end;
    double result;
} ThreadData;

pthread_mutex_t mutex;
double globalProduct = 1.0;

// Funkcja wykonywana przez w¹tki
void* calculateWallis(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    double result = 1.0;

    // Wypisanie informacji o zakresie przetwarzanym przez w¹tek z poprawion¹ kolejnoœci¹ first
    printf("Thread #%ld size = %d first = %d\n",
        pthread_self(), data->end - data->start, data->start);

    // Obliczenia w zakresie
    for (int i = data->start; i <= data->end; i++) {
        double licznik = (4.0 * i * i);
        double mianownik = (2.0 * i - 1) * (2.0 * i + 1);
        result *= licznik / mianownik;
    }

    // Sekcja krytyczna - aktualizacja wspólnej zmiennej globalnej
    pthread_mutex_lock(&mutex);
    globalProduct *= result;
    pthread_mutex_unlock(&mutex);

    // Wypisanie informacji o iloczynie w zakresie
    printf("Thread #%ld prod = %.17f\n", pthread_self(), result);

    pthread_exit(NULL);
}

// Funkcja obliczaj¹ca PI bez u¿ycia w¹tków
double calculatePI(double n) {
    double iloczyn = 1.0;

    for (double i = 1; i <= n; i++) {
        double licznik = (4.0 * i * i);
        double mianownik = (2.0 * i - 1) * (2.0 * i + 1);
        iloczyn *= licznik / mianownik;
    }

    return 2.0 * iloczyn;
}

int main(int argc, char* argv[]) {
// Sprawdzenie poprawnoœci argumentów wywo³ania programu 
    if (argc != 3) {
        printf("U¿ycie: %s <liczba_wyrazow> <liczba_watkow>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int w = atoi(argv[2]);

    if (n <= 1 || n >= 1000000001 || w <= 1 || w >= 101) {
        printf("Niepoprawne argumenty. SprawdŸ warunki dla n i w.\n");
        return EXIT_FAILURE;
    }

    pthread_t threads[MAX_THREADS];
    ThreadData threadData[MAX_THREADS];
    pthread_mutex_init(&mutex, NULL);

// Obliczanie z uzyciem wyników
    int elementsPerThread = n / w;
    int remainingElements = n % w;

    struct timespec start, finish;
    double elapsed;

// Pomiar czasu rozpoczêcia
    clock_gettime(CLOCK_MONOTONIC, &start);

// Tworzenie i uruchamianie w¹tków
    for (int i = 0; i < w; i++) {
        threadData[i].start = i * elementsPerThread + 1;
        threadData[i].end = threadData[i].start + elementsPerThread - 1;

        if (remainingElements > 0) {
            threadData[i].end++;
            remainingElements--;
        }
// Tworzenie w¹tku
        pthread_create(&threads[i], NULL, calculateWallis, (void*)&threadData[i]);
    }

// Oczekiwanie na zakoñczenie w¹tków
    for (int i = 0; i < w; i++) {
        pthread_join(threads[i], NULL);
    }

// Pomiar czasu zakoñczenia
    clock_gettime(CLOCK_MONOTONIC, &finish);

// Obliczenia i wypisanie wyników z u¿yciem w¹tków
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("\nw/Threads: PI=%.17f time=%.5fs\n", globalProduct, elapsed);

// Zwalnianie zasobów muteksu
    pthread_mutex_destroy(&mutex);

// Resetowanie globalnej zmiennej
    globalProduct = 1.0;

// Pomiar czasu rozpoczêcia ponownego obliczenia bez u¿ycia w¹tków
    clock_gettime(CLOCK_MONOTONIC, &start);

// Obliczenia bez u¿ycia w¹tków
    double piWithoutThreads = calculatePI(n);

// Pomiar czasu zakoñczenia ponownego obliczenia
    clock_gettime(CLOCK_MONOTONIC, &finish);

// Obliczenia i wypisanie wyników bez u¿ycia w¹tków
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("wo/Threads: PI = %.17f time = %.5fs\n", piWithoutThreads, elapsed);

    return EXIT_SUCCESS;
}