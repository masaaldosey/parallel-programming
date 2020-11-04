
void compute(double **a, double **b, double **c, double **d, int N, int num_threads) {

  /*TODO:
    Apply loop optimisations to the code below. Think about when certain optimisations are applicable and why.
    For example, when should you apply loop fusion? Should you do this for large problem sizes or small or both?
    Alternatively, does it make sense to break apart a heavy loop with a lot of computations? Why? */
   

#pragma omp parallel for
  for (int i = 0; i < N; i++) 
  {
    for (int j = 0; j < N; j++)
    {
      a[i][j] = 2 * c[i][j];
      d[i][j] = a[i][j] * b[i][j];
    }

    for (int j = 0; j < N-1; j++)
    {
      d[i][j] = d[i][j+1] + c[i][j];
    }
  }

/*  
  for (int i = 0; i < N; i++) {
    for (int j = 0 ; j < N; j++) {
      
    }
  }


  for (int j = 0; j < N-1; j++) {
    for (int i = 0; i < N; i++) {
      d[i][j] = d[i][j+1] + c[i][j];
    }
  } */


}
