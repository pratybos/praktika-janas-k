#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "html_gen.h"

ELO_Ratings player_list[100];
int total_players = sizeof(player_list) / sizeof(player_list[0]);

int comparePeople(const void* a, const void* b) {
    return ((ELO_Ratings*)b)->rating - ((ELO_Ratings*)a)->rating;
}
void sort_elo_list(void) {
    qsort(player_list, total_players, sizeof(ELO_Ratings), comparePeople);
}

void change_player_elo(int p1_rating, int p2_rating) {
    for (int i = 0; i < 100; i++) {
        if (player_list[i].ID == 0) {
            player_list[i].rating = p1_rating;
        }
        if (player_list[i].ID == 1) 
            player_list[i].rating = p2_rating;
    }
}

void populate_elo_list(void) {
    strcpy(player_list[0].name, "Žaidėjas_1");
    player_list[0].rating = 1000;
    player_list[0].ID = 0;
    strcpy(player_list[1].name, "Žaidėjas_2");
    player_list[1].rating = 1000;
    player_list[1].ID = 1;

    for (int i = 2; i < 100; i++) {
        char name[10] = "Testas";
        char num[3];
        sprintf(num, "%d", i);
        strcat(name, num);
        strcpy(player_list[i].name, name);
        player_list[i].rating = GetRandomValue(300, 2500);
        player_list[i].ID = i;
    }
}

void build_html() {     

    FILE *html_file = fopen("report.html", "w");
    if (html_file == NULL) {
        printf("Error opening file!\n");
        return;
    }


    fprintf(html_file, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(html_file, "<<meta charset=\"UTF-8\">\n");
    fprintf(html_file, "    <title>Zaideju ELO</title>\n");
    fprintf(html_file, "    <style>\n");
    fprintf(html_file, "        body { font-family: Arial, sans-serif; margin: 40px; background-color: #f4f4f9; }\n");
    fprintf(html_file, "        h1 { color: #333; }\n");
    fprintf(html_file, "        table { width: 100%%; border-collapse: collapse; margin-top: 20px; background: #fff; }\n");
    fprintf(html_file, "        th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }\n");
    fprintf(html_file, "        th { background-color: #4CAF50; color: white; }\n");
    fprintf(html_file, "        tr:hover { background-color: #f5f5f5; }\n");
    fprintf(html_file, "    </style>\n</head>\n<body>\n");

    fprintf(html_file, "    <h1>Top 100 žaidėjai</h1>\n");
    // fprintf(html_file, "    <p>Generated automatically by a C Program.</p>\n");

    fprintf(html_file, "    <table>\n");
    fprintf(html_file, "        <tr>\n");
    fprintf(html_file, "            <th>Vardas</th>\n");
    fprintf(html_file, "            <th>ELO Reitingas</th>\n");
    fprintf(html_file, "        </tr>\n");

    for (int i = 0; i < total_players; i++) {
        fprintf(html_file, "        <tr>\n");
        fprintf(html_file, "            <td>%s</td>\n", player_list[i].name);
        fprintf(html_file, "            <td>%d</td>\n", player_list[i].rating); 
        fprintf(html_file, "        </tr>\n");
    }

    fprintf(html_file, "    </table>\n");

    fprintf(html_file, "</body>\n</html>\n");
    fclose(html_file);

    printf("Report successfully generated as 'report.html'!\n");
}