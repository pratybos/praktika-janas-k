#pragma once

#include "raylib.h"
#include "defines.h"
#include "board.h"

struct Turn_Move {
    int row;
    int col;
    int direction;
    int color;
};

// extern struct Turn_Move final_move;

struct Board_Bounds GameMove(int arr[MAX][MAX], struct Turn_Move move, bool permanent);