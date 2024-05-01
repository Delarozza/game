#include <curses.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 200
#define HEIGHT 40
#define PLAYER '&'
#define TREASURE '$'
#define DANGER '!'
#define BROWN_PAIR 2
#define WORM 'W'
char field[HEIGHT][WIDTH];
int playerX, playerY;
int score = 0;
int totalTreasures = 0;
int collectedTreasures = 0;
int health = 3;
int level = 0;
int obstaclesCount = 5;
int treasuresCount = 5;
int levelr = 0;

struct Worm {
    int x;
    int y;
    int direction;
};

#define MAX_WORMS 5
struct Worm worms[MAX_WORMS];

void initField() {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            {
                field[y][x] = '#';
            }

        }
    }
}

void drawField() {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y < 5) {
                if ((y >= 1 && y <= 10 && (x == 0 || x == 4)) ||
                    (y == 0 && x >= 1 && x <= 3) ||
                    (y == 10 && x > 1 && x <= 3)) {
                    attron(COLOR_PAIR(1));
                    mvaddch(y, x, '1');
                    attroff(COLOR_PAIR(1));

                } else{
                    if(field[y][x] == PLAYER){
                        attron(COLOR_PAIR(5));
                        mvaddch(y, x, '&');
                        attroff(COLOR_PAIR(5));
                    }
                }
            } else if (y == 5 && x != 1 && x != 2 && x != 3) {
                attron(COLOR_PAIR(3));
                mvaddch(y, x, '=');
                attroff(COLOR_PAIR(3));
            } else if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                attron(COLOR_PAIR(3));
                mvaddch(y, x, 'x');
                attroff(COLOR_PAIR(3));
            } else if (field[y][x] == '!') {
                attron(COLOR_PAIR(2));
                mvaddch(y, x, field[y][x]);
                attroff(COLOR_PAIR(2));
            } else if (field[y][x] == '$') {
                attron(COLOR_PAIR(4));
                mvaddch(y, x, field[y][x]);
                attroff(COLOR_PAIR(4));
            } else if (field[y][x] == PLAYER) {
                attron(COLOR_PAIR(5));
                mvaddch(y, x, PLAYER);
                attroff(COLOR_PAIR(5));
            } else if (field[y][x] == WORM) {
                attron(COLOR_PAIR(3));
                mvaddch(y, x, WORM);
                attroff(COLOR_PAIR(3));
            } else {
                attron(COLOR_PAIR(1));
                mvaddch(y, x, field[y][x]);
                attroff(COLOR_PAIR(1));
            }
        }
    }
    printw("\nScore: %d/%d\n", score, totalTreasures / 2);
    printw("Health: %d\n", health);
    printw("Level: %d", level);
    refresh();
}

void placeElement(char element) {
    int x, y;
    do {
        x = rand() % WIDTH;
        y = rand() % HEIGHT;
    } while (field[y][x] != '#');

    field[y][x] = element;
}


void movePlayer(int x, int y) {
    int targetX = playerX + x;
    int targetY = playerY + y;

    if (targetY >= 1 && targetY < 6 && targetX > 1 && targetX >= 4) {
        return;
    }

    if (targetY >= 1 && targetY < HEIGHT - 1 && targetX >= 1 && targetX < WIDTH - 1 &&
        (field[targetY][targetX] == '#' ||
         field[targetY][targetX] == '$' ||
         field[targetY][targetX] == '!' ||
         field[targetY][targetX] == WORM) ) {

        if (field[targetY][targetX] == DANGER) {
            health--;
            mvprintw(1, 7, "Be careful! You've taken damage!");
            refresh();
            napms(300);
            if (health <= 0) {
                mvprintw(3, 7, "Game Over!");
                refresh();
                getch();
                endwin();
                exit(0);
            }
        } else if (field[targetY][targetX] == TREASURE) {
            score++;
            mvprintw(1, 7, "Great! You found Treasure!");
            refresh();
            napms(300);
        } else if (field[targetY][targetX] == WORM) {
            health--;
            mvprintw(1, 7, "It's worm! Be careful! There are dangerous!");
            refresh();
            napms(300);
        }

        field[playerY][playerX] = ' ';
        playerX = targetX;
        playerY = targetY;
        field[playerY][playerX] = PLAYER;
    }
}



void initWorms() {
    for (int i = 0; i < MAX_WORMS; i++) {
        worms[i].x = rand() % (WIDTH - 2) + 1;
        worms[i].y = rand() % (HEIGHT - 2) + 1;
        worms[i].direction = rand() % 2;
    }
}

void updateWorms() {
    for (int i = 0; i < MAX_WORMS; i++) {
        if (field[worms[i].y][worms[i].x] != PLAYER && field[worms[i].y][worms[i].x] != ' ') {
            field[worms[i].y][worms[i].x] = ' ';
        }

        int randomDirection = rand() % 4;

        // якщо напрямок вліво
        if (randomDirection == 0) {
            if (worms[i].x - 1 >= 1 && (field[worms[i].y][worms[i].x - 1] == '#' || field[worms[i].y][worms[i].x - 1] == ' ') && worms[i].y >= 5) {
                worms[i].x--;
            }
        // якщо напрямок вверх
        } else if (randomDirection == 1) {
            if (worms[i].y - 1 >= 6 && (field[worms[i].y - 1][worms[i].x] == '#' || field[worms[i].y - 1][worms[i].x] == ' ')) {
                worms[i].y--;
            }
        // якщо напрямок вправо
        } else if (randomDirection == 2) {
            if (worms[i].x + 1 < WIDTH - 1 && (field[worms[i].y][worms[i].x + 1] == '#' || field[worms[i].y][worms[i].x + 1] == ' ') && worms[i].y >= 5) {
                worms[i].x++;
            }
        //якщо напрямок вниз
        } else {
            if (worms[i].y + 1 < HEIGHT - 1 && (field[worms[i].y + 1][worms[i].x] == '#' || field[worms[i].y + 1][worms[i].x] == ' ')) {
                worms[i].y++;
            }
        }

        field[worms[i].y][worms[i].x] = WORM;
    }
}

void resetGame() {
    score = 0;
    health = 3;
    level += 1;
    obstaclesCount *= 3;
    treasuresCount *= 2;
    totalTreasures = treasuresCount;

    int minX = 2;
    int maxX = WIDTH - 2;
    int minY = 7;
    int maxY = HEIGHT - 2;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = '#';
        }
    }

    playerX = 1;
    playerY = 4;
    field[playerY][playerX] = PLAYER;

    for (int i = 0; i < obstaclesCount; i++) {
        int x, y;
        do {
            x = rand() % (maxX - minX) + minX;
            y = rand() % (maxY - minY) + minY;
        } while (field[y][x] != '#');
        field[y][x] = DANGER;
    }

      for (int i = 0; i < treasuresCount; i++) {
        int x, y;
        do {
            x = rand() % (maxX - minX) + minX;
            y = rand() % (maxY - minY) + minY;
        } while (field[y][x] != '#');
        field[y][x] = TREASURE;
    }

}

void updateLevel() {
    resetGame();
}

void restartGame() {
    score = 0;
    health = 3;
    level += 1;
    obstaclesCount = 15;
    treasuresCount = 10;
    totalTreasures = treasuresCount;

    int minX = 2;
    int maxX = WIDTH - 2;
    int minY = 7;
    int maxY = HEIGHT - 2;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = '#';
        }
    }


    playerX = 1;
    playerY = 4;
    field[playerY][playerX] = PLAYER;

    for (int i = 0; i < obstaclesCount; i++) {
        int x, y;
        do {
            x = rand() % (maxX - minX) + minX;
            y = rand() % (maxY - minY) + minY;
        } while (field[y][x] != '#');
        field[y][x] = DANGER;
    }

      for (int i = 0; i < treasuresCount; i++) {
        int x, y;
        do {
            x = rand() % (maxX - minX) + minX;
            y = rand() % (maxY - minY) + minY;
        } while (field[y][x] != '#');
        field[y][x] = TREASURE;
    }
}

int main() {
    int gaming = 1;
    srand(time(NULL));
    initscr();
    noecho();
    curs_set(FALSE);
    initField();
    initWorms();
    resetGame();

    while (gaming) {
        drawField();
        updateWorms();

        int ch = getch();
        switch (ch) {
            case 's':
                movePlayer(0, 1);
                break;
            case 'a':
                movePlayer(-1, 0);
                break;
            case 'd':
                movePlayer(1, 0);
                break;
            case 't':
                score += 4;
                break;
            case 'q':
                gaming = 0;
                break;
            case 'r':
                level = 0;
                restartGame();
                break;
            default:
                break;
        }

        if (score >= totalTreasures / 2) {
            mvprintw(3, 7, "New Level reached!!!");
            refresh();
            napms(1000);
            updateLevel();
        }
    }

    endwin();
    return 0;
}
