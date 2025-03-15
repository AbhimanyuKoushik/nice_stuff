#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include "functions.h"

double complex **qr_converge_rayleigh(double complex **A, int dim) {
    double complex **H = makehessberg(A, dim);
    double complex sigma;
    double tolerance = 1e-10;

    for (int m = dim; m > 1; m--) {
        int iterations = 0;

        while (iterations < 20*dim) {
            iterations++;
            sigma = H[m-1][m-1];

            double complex **sigmaI = Matscale(identity(dim), dim, dim, sigma);
            double complex **H_shifted = Matsub(H, sigmaI, dim, dim);

            double complex **Q_T = identity(dim);

            for (int p = 1; p < m; p++) {
                double complex xi = H_shifted[p-1][p-1];
                double complex xj = H_shifted[p][p-1];

                if (cabs(xj) < tolerance) {
                    continue;
                }

                double complex c = conj(xi)/(csqrt(cabs(xi)*cabs(xi)+cabs(xj)*cabs(xj)));
		double complex s = conj(xj)/(csqrt(cabs(xi)*cabs(xi)+cabs(xj)*cabs(xj)));

                double complex **Gi = identity(dim);
                Gi[p-1][p-1] = c;
                Gi[p-1][p] = s;
                Gi[p][p-1] = -conj(s);
                Gi[p][p] = conj(c);

                H_shifted = Matmul(Gi, H_shifted, dim, dim, dim);
                Q_T = Matmul(Q_T, transposeMat(Gi, dim, dim), dim, dim, dim);

                freeMat(Gi, dim);
            }

            H = Matmul(H_shifted, Q_T, dim, dim, dim);
            H = Matadd(H, sigmaI, dim, dim);

            freeMat(sigmaI, dim);
            freeMat(H_shifted, dim);
            freeMat(Q_T, dim);

                if (cabs(H[m-1][m-2]) > tolerance) {
                    continue;
                }
                else{
                	break;
                }
        }
    }

    return H;
}



int main() {
    int n = 3;
    double complex **A = createMat(n, n);
    
    A[0][0] = 1; A[0][1] = 0; A[0][2] = 1;
    A[1][0] = 0; A[1][1] = 1; A[1][2] = 1;
    A[2][0] = 1; A[2][1] = 1; A[2][2] = 1;

    
    printf("Original Matrix:\n");
    printMat(A, n, n);
    printf("\n");
    
    double complex **qr = qr_converge_rayleigh(A, n);
    calcuppereig(qr,n);

    freeMat(A, n);
    freeMat(qr, n);
    return 0;
}
