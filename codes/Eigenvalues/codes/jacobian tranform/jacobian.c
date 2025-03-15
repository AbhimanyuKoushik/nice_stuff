#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"

double **jacobian(double **A, int dim) {
    double **jacobian = copyMat(A, dim, dim);
    double threshold = 1e-10;

    for (int sweep = 0; sweep < 8; sweep++) {
        for (int p = 1; p < dim; p++) {
            for (int q = 0; q < p; q++) {
                if (fabs(jacobian[p][q]) > threshold) {

                    double theta = (jacobian[q][q] - jacobian[p][p]) / (2 * jacobian[p][q]);
                    double sgn = (theta != 0) ? (theta > 0 ? 1 : -1) : 0;
                    double t = sgn / (fabs(theta) + sqrt(theta * theta + 1));
                    double c = 1 / sqrt(t * t + 1);
                    double s = t / sqrt(t * t + 1);

                    double **Ppq = identity(dim);
                    Ppq[p][p] = c;
                    Ppq[p][q] = s;
                    Ppq[q][p] = -s;
                    Ppq[q][q] = c;

                    jacobian = Matmul(jacobian, Ppq, dim, dim, dim);

                    double **PpqT = transposeMat(Ppq, dim, dim);
                    jacobian = Matmul(PpqT, jacobian, dim, dim, dim);

                    freeMat(Ppq, dim);
                    freeMat(PpqT, dim);
                }
                else{
                	jacobian[p][q] = 0;
                }
            }
        }
    }
    return jacobian;
}

int main() {
    int n = 4;
    double **A = createMat(n, n);

    A[0][0] = 1;   A[0][1] = 2;   A[0][2] = 3;  A[0][3] = 4;
    A[1][0] = 2;   A[1][1] = 5;   A[1][2] = 6;  A[1][3] = 7;
    A[2][0] = 3;   A[2][1] = 6;   A[2][2] = 8;  A[2][3] = 9;
    A[3][0] = 4;   A[3][1] = 7;   A[3][2] = 9;  A[3][3] = 10;

    printf("Original Matrix:\n");
    printMat(A, n, n);

    double **jacobianMatrix = jacobian(A,n);
    
    printf("Eigenvalues -->\n");
    for(int i=0; i<n; i++){
    	printf("Eigenvalue %d: %0.9lf\n", i+1, jacobianMatrix[i][i]);
    }

    freeMat(A, n);
    freeMat(jacobianMatrix, n);

    return 0;
}

