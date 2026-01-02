#ifndef ZDIEALANE_DATA_H
#define ZDIEALANE_DATA_H

#define PORT 8080
#define WIDTH 40
#define HEIGHT 20
#define MAX_CLIENTS 4

// Typy správ medzi klientom a serverom
enum MsgType {
    MSG_MOVE = 1,      // klient -> server: smer pohybu
    MSG_STATE = 2,     // server -> klient: celý herný stav
    MSG_GAME_OVER = 3, // server -> klient: koniec hry
    MSG_JOIN_OK = 4,   // server -> klient: úspešné pripojenie
    MSG_PING = 5       // voliteľné
};

// Smer pohybu hada
enum Direction {
    UP = 'w',
    DOWN = 's',
    LEFT = 'a',
    RIGHT = 'd'
};

// Štruktúra herného stavu (posiela sa cez sieť)
typedef struct {
    char grid[HEIGHT][WIDTH];  // ' ' prázdne, '#' stena, 'O' had, '*' jedlo, 'X' kolízia atď.
    int score;
    int game_over;
    // prípadne pridaj viac hadov pre multiplayer
} GameState;

#endif
