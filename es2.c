#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

struct Messaggio {
    char stringa_maiuscola[100];
    int num_vocali;
};

int main(void) {
    int pipe1[2]; 
    int pipe2[2]; 

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(-1);
    }

    pid_t ritorno_fork = fork();
    if (ritorno_fork == -1) {
        perror("fork");
        exit(-2);
    }
    if (!ritorno_fork) {
        close(pipe1[1]); 
        close(pipe2[0]); 

        char buffer[100];
        read(pipe1[0], buffer, sizeof(buffer));
        close(pipe1[0]);
        struct Messaggio msg;
        msg.num_vocali = 0;

        for (int i = 0; buffer[i] != '\0'; i++) {
            msg.stringa_maiuscola[i] = toupper(buffer[i]);
            char c = tolower(buffer[i]);
            if (c == "a"||c=="e"||c == "i"||c == "o"||c == "u") {
                msg.num_vocali++;
            }
            msg.stringa_maiuscola[i + 1] = '\0';
        }
        write(pipe2[1], &msg, sizeof(struct Messaggio));
        close(pipe2[1]);
        exit(0);
    }
    close(pipe1[0]);
    close(pipe2[1]); 
    char input_str[100];
    printf("Inserisci una stringa di testo: ");
    if (fgets(input_str, sizeof(input_str), stdin) != NULL) {
        input_str[strcspn(input_str, "\n")] = 0;
    }
    write(pipe1[1], input_str, sizeof(input_str));
    close(pipe1[1]);

    struct Messaggio risultato;
    read(pipe2[0], &risultato, sizeof(struct Messaggio));
    close(pipe2[0]);

    wait(NULL);

    printf("Stringa maiuscola ricevuta: %s\n", risultato.stringa_maiuscola);
    printf("Numero di vocali presenti: %d\n", risultato.num_vocali);

    return 0;
}