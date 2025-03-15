#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include "functions.h"

// Function to compute Hessenberg form without the sign adjustment step
double complex **qr_converge(double complex **A, int dim) {
    // Copy the original matrix to prevent modifying A directly
    double complex **H = makehessberg(A, dim);
    double tolerance = 1e-12;
    for(int i = 0; i < 20*dim; i++){
    	double complex **Q_T = identity(dim);
    	for (int p = 1; p < dim; p++){
		// Calculate theta, sign, and t
		double complex xi = H[p-1][p-1];
		double complex xj = H[p][p-1];
		if(cabs(xi)<0 && cabs(xi)<0){
			continue;
		}
		double complex c = conj(xi)/(csqrt(cabs(xi)*cabs(xi)+cabs(xj)*cabs(xj)));
		double complex s = conj(xj)/(csqrt(cabs(xi)*cabs(xi)+cabs(xj)*cabs(xj)));

		double complex **Gi = identity(dim);
		Gi[p-1][p-1] = c;
		Gi[p-1][p] = s;
		Gi[p][p-1] = -conj(s);
		Gi[p][p] = conj(c);
		
		H = Matmul(Gi,H,dim,dim,dim);
		Q_T = Matmul(Q_T,transposeMat(Gi,dim,dim),dim,dim,dim);
    	}
    	H = Matmul(H,Q_T,dim,dim,dim);
    }
    return H;
}

void calcuppereig(double complex **A, int dim) {
    int count = 1;
    for (int i = 0; i < dim; i++) {
        if (i < dim - 1 && cabs(A[i + 1][i]) > 1e-12) {
            double complex x1 = A[i][i];
            double complex x2 = A[i + 1][i + 1];
            double complex y1 = A[i + 1][i];
            double complex y2 = A[i][i + 1];

            double complex a = 1.0;
            double complex b = -(x1 + x2);
            double complex c = x1 * x2 - y1 * y2;

            double complex eigenvalue1 = (-b + csqrt(b * b - 4.0 * a * c)) / (2.0 * a);
            double complex eigenvalue2 = (-b - csqrt(b * b - 4.0 * a * c)) / (2.0 * a);

            if(cabs(eigenvalue2)>1e-12){
            	printf("Eigenvalue %d: %.9lf + %.9lfi\n", count, creal(eigenvalue1), cimag(eigenvalue1));
            }
            else{
            	printf("Eigenvalue %d: 0.000000000 + 0.000000000i\n", count);
            }
            
            if(cabs(eigenvalue2)>1e-12){
            	printf("Eigenvalue %d: %.9lf + %.9lfi\n", count + 1, creal(eigenvalue2), cimag(eigenvalue2));
            }
            else{
            	printf("Eigenvalue %d: 0.000000000 + 0.000000000i\n", count+1);
            }
            count += 2;

            i++;
        }
        else{
        	if(cabs(A[i][i])>1e-12){
            		printf("Eigenvalue %d: %.9lf + %.9lfi\n", count, creal(A[i][i]), cimag(A[i][i]));
            	}
            	else{
            		printf("Eigenvalue %d: 0.000000000 + 0.000000000i\n", count);
            	}
            count++;
        }
    }
}

int main() {
    int n = 3;
    double complex **A = createMat(n, n);
    
    A[0][0] = 1; A[0][1] = 0; A[0][2] = 1;
    A[1][0] = 0; A[1][1] = 1; A[1][2] = 1;
    A[2][0] = 1; A[2][1] = 1; A[2][2] = 1;

    double complex **qr = qr_converge(A,n);	
    calcuppereig(qr,n);
    
    freeMat(A, n);
    freeMat(qr, n);
    return 0;
}
