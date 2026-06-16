#pragma once

struct Tile_Amount {
    int leaves;
    int branches;
};

// struct Settings {
//     int leaf_amount;
//     int branch_amount;
//     enum Board_Start {VERTICAL, HORIZONTAL, PLUS, U} selected_start;
//     bool clockwise_mode;
//     enum Turn_Order {ABAB, ABBA, AABB} turn_order;    
// };

extern struct Tile_Amount tile_amount;
// extern struct Settings game_settings;