#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include "functions.h"

double complex **makehessberg(double complex **A, int dim) {
    double complex **toreturn = copyMat(A, dim, dim);

    for (int k = 0; k < dim - 2; k++) {
        int u_dim = dim - k - 1;
        double complex **u = createMat(u_dim, 1);

        for (int i = 0; i < u_dim; i++) {
            u[i][0] = toreturn[k + 1 + i][k];
        }
        double complex rho;
        if(cabs(u[0][0])!=0){
        	rho = - u[0][0] / cabs(u[0][0]);
        }
        else{
        	rho = 1;
        }
        u[0][0] += -1 * rho * Matnorm(u, u_dim);
        
        double norm_u = Matnorm(u, u_dim);
        if (norm_u != 0.0) {
         	   for (int i = 0; i < u_dim; i++) {
                u[i][0] /= norm_u;
            }
        }
        else{
        	continue;
        }

        double complex **Hk = identity(dim);
        double complex **u_transpose = transposeMat(u, u_dim, 1);
        double complex **u_ut = Matmul(u, u_transpose, u_dim, 1, u_dim);

        for (int i = 0; i < u_dim; i++) {
            for (int j = 0; j < u_dim; j++) {
                Hk[k + 1 + i][k + 1 + j] -= 2 * u_ut[i][j];
            }
        }

        toreturn = Matmul(Hk, toreturn, dim, dim, dim);
        toreturn = Matmul(toreturn, Hk, dim, dim, dim);
        
        freeMat(u, u_dim);
        freeMat(u_transpose, 1);
        freeMat(u_ut, u_dim);
        freeMat(Hk, dim);
    }
    return toreturn;
}

int main() {
    int n = 4;
    double complex **A = createMat(n, n);

    A[0][0] = 1 + 1 * I;   A[0][1] = 2 + 5 * I;   A[0][2] = 3 + 9 * I;    A[0][3] = 4 + 13 * I;
    A[1][0] = 1 + 1 * I;   A[1][1] = 6 + 6 * I;   A[1][2] = 7 + 10 * I;   A[1][3] = 8 + 14 * I;
    A[2][0] = 0 + 0 * I;   A[2][1] = 10 + 7 * I;  A[2][2] = 11 + 11 * I;  A[2][3] = 12 + 15 * I;
    A[3][0] = 0 + 0 * I;   A[3][1] = 14 + 8 * I;  A[3][2] = 15 + 12 * I;  A[3][3] = 16 + 16 * I;
    
    printf("Original Matrix:\n");
    printMat(A, n, n);

    // Compute Hessenberg form without sign adjustment
    double complex **hessenberg = makehessberg(A, n);
    printf("Hessenberg Matrix:\n");
    printMat(hessenberg, n, n);

    freeMat(A, n);
    freeMat(hessenberg, n);
    return 0;
}

