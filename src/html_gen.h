#pragma once

typedef struct {
    int ID;
    char name[30];
    int rating;
} ELO_Ratings;

extern ELO_Ratings player_list[100];

void build_html();
void populate_elo_list();
void sort_elo_list();
void change_player_elo(int p1_rating, int p2_rating);