#pragma once

#include "defines.h"
#include "board.h"

struct Turn_Move {
    int row;
    int col;
    int direction;
    int color;
};

typedef enum {
    PERMANENT,
    THEORETICAL
} Turn_Type;

// extern struct Turn_Move final_move;

struct Board_Bounds GameMove(int arr[MAX][MAX], struct Turn_Move move, Turn_Type mode);