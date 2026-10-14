#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(-1);
    }
    pid_t pid_b = fork();
    if (pid_b == -1) {
        perror("fork B");
        exit(-2);
    }
    if (pid_b == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        int num;
        while (read(pipe1[0], &num, sizeof(int)) > 0) {
            if (num >= 50) {
                int quadrato = num * num;
                write(pipe2[1], &quadrato, sizeof(int));
            }
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    pid_t pid_c = fork();
    if (pid_c == -1) {
        perror("fork C");
        exit(-2);
    }

    if (pid_c == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);

        int num;
        int somma = 0;
        int conteggio = 0;
        printf("Numeri ricevuti ed elaborati da C (quadrati >= 50):\n");
        while (read(pipe2[0], &num, sizeof(int)) > 0) {
            printf("%d ", num);
            somma += num;
            conteggio++;
        }
        printf("\n");

        if (conteggio > 0) {
            double media = (double)somma / conteggio;
            printf("Media aritmetica: %.2f\n", media);
        } else {
            printf("Nessun numero valido ricevuto (nessun numero >= 50).\n");
        }
        close(pipe2[0]);
        exit(0);
    }

    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);
    srand(time(NULL));
    for (int i = 0; i < 20; i++) {
        int casuale = (rand() % 100) + 1;
        write(pipe1[1], &casuale, sizeof(int));
    }

    close(pipe1[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}