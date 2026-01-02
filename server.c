#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "zdielaneData.h"

GameState game;
pthread_mutex_t game_mutex = PTHREAD_MUTEX_INITIALIZER;
int clients[MAX_CLIENTS];
int client_count = 0;

void init_game() {
    memset(game.grid, ' ', sizeof(game.grid));
    // jednoduché okraje
    for (int i = 0; i < WIDTH; i++) {
        game.grid[0][i] = game.grid[HEIGHT-1][i] = '#';
    }
    for (int i = 0; i < HEIGHT; i++) {
        game.grid[i][0] = game.grid[i][WIDTH-1] = '#';
    }
    // TODO: spawn hada a jedla
    game.score = 0;
    game.game_over = 0;
}

void* handle_client(void* arg) {
    int sock = *(int*)arg;
    // pošli úvodný stav
    send(sock, &game, sizeof(game), 0);

    char dir;
    while (recv(sock, &dir, 1, 0) > 0 && !game.game_over) {
        // TODO: spracuj pohyb podľa dir ('w','a','s','d')
        // aktualizuj game.grid, score, deteguj kolíziu

        pthread_mutex_lock(&game_mutex);
        // broadcast nového stavu všetkým klientom
        for (int i = 0; i < client_count; i++) {
            if (clients[i] != -1) {
                send(clients[i], &game, sizeof(game), 0);
            }
        }
        pthread_mutex_unlock(&game_mutex);
    }
    close(sock);
    return NULL;
}

void run_server() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    init_game();
    srand(time(NULL));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    printf("Server beží na porte %d...\n", PORT);

    while (1) {
        int client_sock = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = client_sock;
            pthread_t thread;
            int* pclient = malloc(sizeof(int));
            *pclient = client_sock;
            pthread_create(&thread, NULL, handle_client, pclient);
        } else {
            close(client_sock);
        }
    }
}
