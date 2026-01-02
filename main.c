#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "zdielaneData.h"

extern void run_server();
extern void run_client(const char* ip);

int main() {
    int choice;
    char ip[16] = "127.0.0.1";

    printf("=== SNAKE KLIENT-SERVER ===\n");
    printf("1. Vytvoriť hru (stať sa hostom)\n");
    printf("2. Pripojiť sa k hre\n");
    printf("Voľba: ");
    scanf("%d", &choice);

    if (choice == 1) {
        pid_t pid = fork();
        if (pid == 0) {
            run_server();
            return 0;
        } else if (pid > 0) {
            // rodič = klient
            sleep(1);  // počkaj kým sa server naštartuje
            run_client("127.0.0.1");
        } else {
            perror("fork");
        }
    } else if (choice == 2) {
        printf("Zadaj IP servera (default 127.0.0.1): ");
        scanf("%s", ip);
        if (strlen(ip) == 0) strcpy(ip, "127.0.0.1");
        run_client(ip);
    }

    return 0;
}
