#pragma once

#include "raylib.h"
#include "defines.h"

struct Animation {
    float push_offset;
    Vector2 push_direction;
    Color tile_tint;
};

typedef enum {
    STATE_IDLE,
    STATE_MADE_MOVE,
    STATE_ANIMATING,
    STATE_POST_ANIMATION
} TileState;

extern struct Animation animation[MAX][MAX];