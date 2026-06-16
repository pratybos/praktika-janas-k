#include "cpu_move.h"
#include "raylib.h"
#include <string.h>
#include <time.h>
#include "game_move.h"
#include "score.h"
#include "game_settings.h"

struct Moves* InsertToBack(struct Moves* head, int direction, int row, int col) {
    struct Moves* temp = (struct Moves*)MemAlloc(sizeof(struct Moves));
    temp->row = row;
    temp->col = col;
    temp->direction = direction;
    temp->score = 0;
    temp->next = NULL;
    if (head == NULL)
        head = temp;
    else {
        struct Moves* temp1 = head;
        while (temp1->next != NULL)
            temp1 = temp1->next;
        temp1->next = temp;
    }
    return head;
}

struct Moves* Destroy_List(struct Moves* head) {
    struct Moves *current = head;
    struct Moves *temp;

    while (current != NULL) {
        temp = current->next;
        MemFree(current);
        current = temp;
    }
    return NULL;
}

struct Turn_Move CpuMoves(int board[MAX][MAX], int turn_color, struct Moves* head, int cpu_difficulty) { 
    /*
    1. Trinamas susietas ejimu sarasas (Moves)
    2. Visi galimi ejimai surasomi i susieta sarasa
    3. Ciklas:
        Laikinas lentos masyvas kopijuojamas is pagrindinio masyvo board
        Ejimas is saraso atliekamas nukopijuotame masyve
        Skaiciuojamas tasku pokytis pries ir po ejimo
        Issaugomas didziausias tasku pokytis i max_score
    4. Is susieto saraso Moves trinami ejimai kuriu verte mazesne uz max_score
    5. Is likusiu ejimu sarase pasirenkamas atsitiktinis
    6. Pasirinktas ejimas pritaikomas pagrindiniam masyvui "board"
    */

    head = Destroy_List(head);

    int list_length = 0;
    for (int row = bounds.row_min - 1; row <= bounds.row_max + 1; row++) {
        for (int col = bounds.col_min - 1; col <= bounds.col_max + 1; col++) {
            if (board[row][col] == ARROW_LEFT || 
                board[row][col] == ARROW_RIGHT || 
                board[row][col] == ARROW_UP || 
                board[row][col] == ARROW_DOWN ) {
                    head = InsertToBack(head, board[row][col], row, col);
                    list_length++;
            }
        }
    }
    
    struct Moves* current = head, *prev;
    struct Score score_after;

    struct Turn_Move test_move;

    int theory_board[MAX][MAX];
    
    int max_score = -tile_amount.leaves;
    while (current != NULL) {
        memcpy(theory_board, board, sizeof(board[0][0]) * MAX * MAX); 

        test_move.col = current->col;
        test_move.row = current->row;
        test_move.direction = current->direction;
        test_move.color = turn_color;

        GameMove(theory_board, test_move, THEORETICAL);
        score_after = CheckScore(theory_board, CHECK_SCORE);
        if (turn_color == RED_LEAF)
            current->score = score_after.red - score_after.green;
        else
            current->score = score_after.green - score_after.red;
            
        if (current->score > max_score)
            max_score = current->score;

        current = current->next;
    }

    if (cpu_difficulty == 1)
        max_score -= 2;

    current = head;
    prev = head;
    while (current != NULL) {
        if (current->score < max_score) {
            if (current == head) {
                head = current->next;
                MemFree(current);
                current = head;
                prev = head;
            }
            else {
                prev->next = current->next;
                MemFree(current);
                current = prev->next;
            }
            list_length--;
        }
        else {
            prev = current;
            current = current->next;
        }
    }    
     
    SetRandomSeed(time(NULL));
    int random_move = GetRandomValue(1, list_length);
    
    current = head;
    for (int n = 1; n < random_move; n++)
        current = current->next;

    struct Turn_Move result;

    result.row = current->row;
    result.col = current->col;
    result.direction = current->direction;
    result.color = turn_color;

    return result;
}