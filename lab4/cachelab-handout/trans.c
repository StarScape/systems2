/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

void transpose_64(int M, int N, int A[N][M], int B[M][N]);
int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }    
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}

void transpose_64(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, k, l;
  int tmp0, tmp1, tmp2, tmp3;
  int tmp4, tmp5, tmp6, tmp7;

  // Here be dragons...
  for (i = 0; i < N; i += 8) {
    for (j = 0; j < M; j += 8) {
      for (k = i; k < i+4; k++) {
        tmp0 = A[k][j+0];
        tmp1 = A[k][j+1];
        tmp2 = A[k][j+2];
        tmp3 = A[k][j+3];
        tmp4 = A[k][j+4];
        tmp5 = A[k][j+5];
        tmp6 = A[k][j+6];
        tmp7 = A[k][j+7];

        B[j+0][k] = tmp0;
        B[j+1][k] = tmp1;
        B[j+2][k] = tmp2;
        B[j+3][k] = tmp3;

        
        B[j+0][k+4] = tmp4;
        B[j+1][k+4] = tmp5;
        B[j+2][k+4] = tmp6;
        B[j+3][k+4] = tmp7;
      }
   
      for (k = j; k < j+4; k++) {
        tmp4 = A[i+4][k];
        tmp5 = A[i+5][k];
        tmp6 = A[i+6][k];
        tmp7 = A[i+7][k];
        
        tmp0 = B[k][i+4];
        tmp1 = B[k][i+5];
        tmp2 = B[k][i+6];
        tmp3 = B[k][i+7];

        B[k][i+4] = tmp4;
        B[k][i+5] = tmp5;
        B[k][i+6] = tmp6;
        B[k][i+7] = tmp7;
        
        B[k+4][i+0] = tmp0;
        B[k+4][i+1] = tmp1;
        B[k+4][i+2] = tmp2;
        B[k+4][i+3] = tmp3;
        
        for (l = 0; l < 4; l++) {
          B[k+4][i+l+4] = A[i+l+4][k+4];
        }
      }
    }
  }
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, k, l, b = 16;
  int tmp, d;

  if (N == 32 && M == 32) {
    b = 8;
  }

  if (N == 64 && M == 64) {
    transpose_64(M, N, A, B);
  }
  else {
    for (i = 0; i < N; i += b) {
      for (j = 0; j < M; j += b) {
        for (k = 0; k < b && i + k < N; k++) {
          for (l = 0; l < b && j + l < M; l++) {
            if (j+l != i+k) {
              B[j+l][i+k] = A[i+k][j+l];
            }
            else {
              tmp = A[i+k][j+l];
              d = i+k; 
            }
          }
          
          if (i == j) {
           B[d][d] = tmp;
          }
        }
      }
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
}
