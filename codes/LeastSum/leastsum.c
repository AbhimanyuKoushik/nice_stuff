#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INF INT_MAX

typedef struct {
    int xcord;
    int ycord;
    double val;
} point;

typedef struct {
    int size;
    point **gridpoints;
} grid;

grid constructgrid(int size) {
    grid g;
    g.size = size;

    g.gridpoints = (point **)malloc(size * sizeof(point *));
    for (int i = 0; i < size; i++) {
        g.gridpoints[i] = (point *)malloc(size * sizeof(point));
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            g.gridpoints[i][j].xcord = i;
            g.gridpoints[i][j].ycord = j;
            g.gridpoints[i][j].val = ((double)rand() / RAND_MAX);
        }
    }

    return g;
}

void printgrid(grid g) {
    for (int i = 0; i < g.size - 1; i++) {
        for (int j = 0; j < g.size - 1; j++) {
            printf("%.2f -- ", g.gridpoints[i][j].val);
        }
        printf("%.2f", g.gridpoints[i][g.size-1].val);
        printf("\n");
        for (int j = 0; j < g.size; j++) {
            printf("  |     ");
        }
        printf("\n");
    }
    for (int j = 0; j < g.size - 1; j++) {
        printf("%.2f -- ", g.gridpoints[g.size-1][j].val);
    }

    printf("%.2f", g.gridpoints[g.size-1][g.size-1].val);
    printf("\n");
}

void freegrid(grid g) {
    for (int i = 0; i < g.size; i++) {
        free(g.gridpoints[i]);
    }
    free(g.gridpoints);
}

*points dijkstra(point startpoint, point endpoint, grid grid){
    int distance[grid.size][grid.size] = {INF};
    point Visitedpoints[gird.size * grid.size] = {NULL};
    distance[startpoint.x][startpoint.y] = 0;
};

int main() {
    srand(time(NULL));
    

    int size = 10;
    grid g = constructgrid(size);
    printgrid(g);

    int x0, y0, xn, yn;
    printf("Enter the Coordinate of Starting point in the form x y: ")
    scanf("%d", &x0);
    scanf("%d", &y0);


    printf("Enter the Coordinate of End point in the form x y: ")
    scanf("%d", &xn);
    scanf("%d", &yn);

    point startpoint, endpoint;
    startpoint = {x0, y0, gridpoints[x0][y0]};
    endpoint = {xn, yn, gridpoints[xn][yn]};

    point *path;
    path = dijkstra(startpoint, endpoint, g);
    printsolvedgrid(path, g);
    freegrid(g);

    return 0;
}
