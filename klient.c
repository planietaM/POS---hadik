#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include <arpa/inet.h>
#include "zdielaneData.h"

// globálny socket
int sock = -1;

void clear_screen() {
    printf("\033[2J\033[H");  // ANSI escape pre clear
}

void draw_game(GameState* state) {
    clear_screen();
    printf("Score: %d\n", state->score);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            putchar(state->grid[i][j]);
        }
        putchar('\n');
    }
    if (state->game_over) {
        printf("\n=== GAME OVER ===\n");
    }
    fflush(stdout);
}

// vlákno na čítanie vstupu bez blokovania enteru
void* input_thread(void* arg) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (1) {
        char c = getchar();
        if (c == 'q') break;
        if (strchr("wasd", c)) {
            send(sock, &c, 1, 0);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return NULL;
}

void* render_thread(void* arg) {
    GameState state;
    while (recv(sock, &state, sizeof(state), 0) > 0) {
        draw_game(&state);
        if (state.game_over) {
            sleep(3);
            break;
        }
    }
    return NULL;
}

void run_client(const char* ip) {
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Pripojenie zlyhalo");
        exit(1);
    }

    pthread_t input_t, render_t;
    pthread_create(&input_t, NULL, input_thread, NULL);
    pthread_create(&render_t, NULL, render_thread, NULL);

    pthread_join(render_t, NULL);  // čakáme na koniec hry
    pthread_cancel(input_t);

    close(sock);
}
