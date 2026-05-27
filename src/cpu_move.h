#pragma once

#include "defines.h"

struct Moves {
    int row;
    int col;
    int direction;
    int score;
    struct Moves* next;
};

// extern struct Moves* head;

struct Moves* InsertToBack(struct Moves* head, int direction, int row, int col);
struct Moves* Destroy_List(struct Moves* head);
struct Turn_Move CpuMoves(int board[MAX][MAX], int theory_board[MAX][MAX], int turn_color, struct Moves* head, int cpu_difficulty);