#pragma once

#include "raylib.h"
#include "defines.h"

struct Score {
    int green;
    int red;
};

struct Score CheckScore(int arr[MAX][MAX], bool game_finished);