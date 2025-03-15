// Function prototypes
double **createMat(int m, int n);
double **Matadd(double **a, double **b, int m, int n);
double **Matscale(double **a, int m, int n, double k);  // Added 'k' parameter
double norm(double **a, int m);
void printMat(double **p, int m, int n);
double **transposeMat(double **a, int m, int n);
double Matdot(double **a, double **b, int m);
double **Matunit(double **a, int m);
double **Matmul(double **a, double **b, int m, int n, int p);
double **Matcol(double **a, int m, int n);
double **Matsub(double **a, double **b, int m, int n);
void Matassign(double **a, double **b, int m, int n);


double **createMat(int m,int n){
	int i;
	double **a;
 
	a = (double **)malloc(m * sizeof( *a));
	for (i=0; i<m; i++){
		a[i] = (double *)malloc(n * sizeof( *a[i]));
	}
	return a;
}

double **Matadd(double **a,double **b, int m, int n){
	int i, j;
	double **c;
	c = createMat(m,n);
	
	for(i=0;i<m;i++){
  		for(j=0;j<n;j++){
			c[i][j]= a[i][j]+b[i][j];
		}
	}
	return c;
}

double **Matscale(double **a, int m, int n, double k){
	int i, j;
	double **c;
	c = createMat(m,n);
	for(i=0;i<m;i++){
  		for(j=0;j<n;j++){
			c[i][j]= k*a[i][j];
		}
	}
	return c;
}

double Matnorm(double **a,int m){
	double temp=0;
	for(int i=0; i<m;i++){
		temp+=a[i][0]*a[i][0];
	}
	return temp;
}

void printMat(double **p, int m,int n){
	int i,j;
	for(i=0;i<m;i++){
  		for(j=0;j<n;j++){
  			printf("%lf ",p[i][j]);
  		}
 	printf("\n");
	}
}

double **transposeMat(double **a,  int m, int n){
	int i, j;
	double **c;
	c = createMat(n,m);
	for(i=0;i<n;i++){
  		for(j=0;j<m;j++){
			c[i][j]= a[j][i];
		}
	}
	return c;
}

double Matdot(double **a, double ** b, int m){
	double **temp= Matmul(transposeMat(a,m,1),b,1,m,1);
	return temp[0][0];
}

double **Matunit(double **a,int m){
	double **c=createMat(m,1);
	double temp = Matnorm(a,m);
	for(int i = 0; i < m; i++){
		c[i][0] = a[i][0]/temp; 
	}
	return c;
}


double **Matmul(double **a, double **b, int m, int n, int p){
	int i, j, k;
	double **c, temp =0;
	c = createMat(m,p);
	for(i=0;i<m;i++){
		for(k=0;k<p;k++){
			for(j=0;j<n;j++){
				temp= temp+a[i][j]*b[j][k];
			}
		c[i][k]=temp;
		temp = 0;
		}
	}
	return c;
}

double **Matcol(double **a,int m, int n){
	int i = 0;
	double **b = createMat(m,1);
	for (i = 0; i < m; i++){
		b[i][0] = a[i][n];
	}
return b;
}

double **Matsub(double **a,double **b, int m, int n){
	int i, j;
	double **c;
	c = createMat(m,n);
	for(i=0;i<m;i++){
  		for(j=0;j<n;j++){
			c[i][j]= a[i][j]-b[i][j];
		}
	}
	return c;
}

double **identity(int dim) {
    double **A = createMat(dim, dim);
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            A[i][j] = (i == j) ? 1 : 0;
        }
    }
    return A;
}

double **copyMat(double **src, int m, int n) {
    double **dest = createMat(m, n);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            dest[i][j] = src[i][j];
        }
    }
    return dest;
}

void freeMat(double **a, int m) {
    for (int i = 0; i < m; i++) {
        free(a[i]);
    }
    free(a);
}
