/* The Algorithm used in this code is known as DLX algorithm created by Knuth's
The Algorithm works in the following manner ->
1. Convert the sudoku into a sparse matrix (entries only 0 and 1) in the following manner -
    1) The number of rows include every possible digit in each cell which is not filled, ( Maximum number of rows = 9x81 = 729)
    2) The number of columns include contraint for each cell. The constriants are -
        - There is only 1 digit in each of the cell
        - A number appears only once in each row
        - A number appears only once in each column
        - A number appears only once in each 3x3 box
    3) The number of columns are each constraint for each digit. So, the number of columns are 4x81 = 384
    4) Fill the matrix such that an entry is 1 if it the digit satisfies a particular constraint when placed in a particular cell
    5) If it doesn't satisfy, it is 0.
2. Convert the matrix into a network of Doubly linked list with the following rules ->

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define SIZE 9
#define MAX_ROWS 729
#define COLS 324

typedef struct DLXNode {
    struct DLXNode *left, *right, *up, *down;
    struct ColumnHeader *column;
    uint16_t row_idx; 
} DLXNode;

typedef struct ColumnHeader {
    DLXNode node;
    uint16_t number_of_ones;  // can exceed 255
    uint16_t index;           // up to 324
} ColumnHeader;

static DLXNode *solution[MAX_ROWS];
static uint16_t solution_count = 0;  // max 729

// To track (r, c, d) for each sparse row
typedef struct {
    uint8_t row, col, num;  // always ≤ 9
} RowInfo;
RowInfo row_infos[MAX_ROWS];

ColumnHeader *GetColumnHeaders(void) {
    ColumnHeader *master = malloc(sizeof(ColumnHeader));
    master->node.left = master->node.right = &master->node;
    master->node.up = master->node.down = &master->node;
    master->node.column = master;
    master->number_of_ones = 0;
    master->index = (uint16_t)-1;

    ColumnHeader *prev = master;
    for (uint16_t i = 0; i < COLS; i++) {
        ColumnHeader *col = malloc(sizeof(ColumnHeader));
        col->index = i;
        col->node.left = &prev->node;
        prev->node.right = &col->node;
        col->node.up = col->node.down = &col->node;
        col->node.column = col;
        col->number_of_ones = 0;
        prev = col;
    }
    prev->node.right = &master->node;
    master->node.left = &prev->node;
    return master;
}

void InitialiseStructure(ColumnHeader *master, uint8_t sparse[MAX_ROWS][COLS], uint16_t row_count) {
    ColumnHeader *col_headers[COLS];
    DLXNode *hdr = master->node.right;
    for (uint16_t c = 0; c < COLS; c++) {
        col_headers[c] = hdr->column;
        hdr = hdr->right;
    }

    for (uint16_t r = 0; r < row_count; r++) {
        DLXNode *row_start = NULL, *row_prev = NULL;
        for (uint16_t c = 0; c < COLS; c++) {
            if (!sparse[r][c]) continue;
            DLXNode *node = malloc(sizeof(DLXNode));
            node->column = col_headers[c];
            node->row_idx = r;
            node->down = &col_headers[c]->node;
            node->up = col_headers[c]->node.up;
            col_headers[c]->node.up->down = node;
            col_headers[c]->node.up = node;
            col_headers[c]->number_of_ones++;
            if (!row_start) {
                row_start = node;
                node->left = node->right = node;
            } else {
                node->left = row_prev;
                node->right = row_start;
                row_prev->right = node;
                row_start->left = node;
            }
            row_prev = node;
        }
    }
}

void Cover(ColumnHeader *col) {
    col->node.right->left = col->node.left;
    col->node.left->right = col->node.right;
    for (DLXNode *r = col->node.down; r != &col->node; r = r->down) {
        for (DLXNode *j = r->right; j != r; j = j->right) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->column->number_of_ones--;
        }
    }
}

void Uncover(ColumnHeader *col) {
    for (DLXNode *r = col->node.up; r != &col->node; r = r->up) {
        for (DLXNode *j = r->left; j != r; j = j->left) {
            j->column->number_of_ones++;
            j->down->up = j;
            j->up->down = j;
        }
    }
    col->node.right->left = &col->node;
    col->node.left->right = &col->node;
}

bool Search(ColumnHeader *root, uint16_t depth) {
    if (root->node.right == &root->node) {
        solution_count = depth;
        return true;
    }

    ColumnHeader *selected_column = NULL;
    int min_size = INT_MAX;
    for (DLXNode *col_node = root->node.right; col_node != &root->node; col_node = col_node->right) {
        ColumnHeader *col_header = col_node->column;
        if ((int)col_header->number_of_ones < min_size) {
            min_size = col_header->number_of_ones;
            selected_column = col_header;
        }
    }

    if (!selected_column || selected_column->number_of_ones == 0) return false;

    Cover(selected_column);

    for (DLXNode *row_node = selected_column->node.down; row_node != &selected_column->node; row_node = row_node->down) {
        solution[depth] = row_node;

        for (DLXNode *right_node = row_node->right; right_node != row_node; right_node = right_node->right) {
            Cover(right_node->column);
        }

        if (Search(root, depth + 1)) return true;

        for (DLXNode *left_node = row_node->left; left_node != row_node; left_node = left_node->left) {
            Uncover(left_node->column);
        }
    }

    Uncover(selected_column);

    return false;
}

uint16_t get_sparse_matrix(uint8_t sudoku[SIZE][SIZE], uint8_t sparse[MAX_ROWS][COLS]) {
    uint16_t row_count = 0;
    for (uint8_t r = 0; r < SIZE; r++) {
        for (uint8_t c = 0; c < SIZE; c++) {
            for (uint8_t d = 0; d < SIZE; d++) {
                if (sudoku[r][c] && sudoku[r][c] != d + 1) continue;
                for (uint16_t j = 0; j < COLS; j++) sparse[row_count][j] = 0;
                sparse[row_count][9 * r + c] = 1;
                sparse[row_count][81 + 9 * r + d] = 1;
                sparse[row_count][162 + 9 * c + d] = 1;
                uint8_t b = (r / 3) * 3 + (c / 3);
                sparse[row_count][243 + 9 * b + d] = 1;
                row_infos[row_count] = (RowInfo){r, c, d + 1};
                row_count++;
            }
        }
    }
    return row_count;
}

void extract_solution(uint8_t sudoku[SIZE][SIZE]) {
    for (uint16_t i = 0; i < solution_count; i++) {
        uint16_t r_idx = solution[i]->row_idx;
        uint8_t r = row_infos[r_idx].row;
        uint8_t c = row_infos[r_idx].col;
        uint8_t d = row_infos[r_idx].num;
        sudoku[r][c] = d;
    }
}

void print_sudoku(uint8_t sudoku[SIZE][SIZE]) {
    for (uint8_t i = 0; i < SIZE; i++) {
        for (uint8_t j = 0; j < SIZE; j++) printf("%d ", sudoku[i][j]);
        printf("\n");
    }
}

int main(void) {
    uint8_t sudoku[SIZE][SIZE] = {
        {1, 3, 2, 0, 8, 4, 0, 9, 0},
        {0, 8, 0, 7, 0, 0, 0, 0, 0},
        {0, 0, 5, 1, 0, 2, 0, 0, 3},
        {0, 9, 0, 8, 0, 7, 0, 5, 1},
        {0, 5, 0, 0, 0, 0, 0, 4, 0},
        {8, 1, 0, 9, 0, 5, 0, 7, 0},
        {9, 0, 0, 5, 0, 6, 1, 0, 0},
        {0, 0, 0, 0, 0, 9, 0, 2, 0},
        {0, 2, 0, 3, 7, 0, 9, 6, 5}
    };

    uint8_t sparse[MAX_ROWS][COLS];
    uint16_t rows = get_sparse_matrix(sudoku, sparse);
    ColumnHeader *master = GetColumnHeaders();
    InitialiseStructure(master, sparse, rows);
    if (Search(master, 0)) {
        extract_solution(sudoku);
        printf("Solved Sudoku:\n");
        print_sudoku(sudoku);
    } else {
        printf("No solution found.\n");
    }
    return 0;
}
