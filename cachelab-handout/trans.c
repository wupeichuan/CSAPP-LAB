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

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int b1,b2,b3,b4,b5,b6,b7,b8;
    for(int i=0;i<N;i+=8)
        for(int j=0;j<M;j+=8)
        {
            if((i+8<N)&&(j+8<M))
            {
            for(int p=0;p<4;p++){
                b1=A[p+i][j];b2=A[p+i][j+1];b3=A[p+i][j+2];b4=A[p+i][j+3];b5=A[p+i][j+4];b6=A[p+i][j+5];b7=A[p+i][j+6];b8=A[p+i][j+7];
                B[j][p+i]=b1;B[j+1][p+i]=b2;B[j+2][p+i]=b3;B[j+3][p+i]=b4;B[j][p+i+4]=b5;B[j+1][p+i+4]=b6;B[j+2][p+i+4]=b7;B[j+3][p+i+4]=b8;
            }
            for(int p=0;p<4;p++){
                b1=A[i+4][j+p];b2=A[i+5][j+p];b3=A[i+6][j+p];b4=A[i+7][j+p];b5=B[j+p][i+4];b6=B[j+p][i+5];b7=B[j+p][i+6];b8=B[j+p][i+7];
                B[j+p][i+4]=b1;B[j+p][i+5]=b2;B[j+p][i+6]=b3;B[j+p][i+7]=b4;B[j+4+p][i]=b5;B[j+4+p][i+1]=b6;B[j+4+p][i+2]=b7;B[j+4+p][i+3]=b8;
            }
            for(int p=4;p<8;p++){
                b1=A[p+i][j+4];b2=A[p+i][j+5];b3=A[p+i][j+6];b4=A[p+i][j+7];
                B[j+4][p+i]=b1;B[j+5][p+i]=b2;B[j+6][p+i]=b3;B[j+7][p+i]=b4;
            }
            }
            else if((i+8<N)&&(j+8>=M))
            {
                for(int p=0;p<4;p++){
                    b1=A[i+p][j];b2=A[i+p][j+1];b3=A[i+p][j+2];b4=A[i+p][j+3];b5=A[i+p][j+4];
                    B[j][i+p]=b1;B[j+1][i+p]=b2;B[j+2][i+p]=b3;B[j+3][i+p]=b4;B[j+4][i+p]=b5;
                }
                for(int p=4;p<8;p++){
                    b1=A[i+p][j];b2=A[i+p][j+1];b3=A[i+p][j+2];b4=A[i+p][j+3];b5=A[i+p][j+4];
                    B[j][i+p]=b1;B[j+1][i+p]=b2;B[j+2][i+p]=b3;B[j+3][i+p]=b4;B[j+4][i+p]=b5;
                }
            }
            else if((i+8>=N)&&(j+8<M))
            {
                for(int q=0;q<4;q++){
                    b1=A[i][j+q];b2=A[i+1][j+q];b3=A[i+2][j+q];
                    B[j+q][i]=b1;B[j+q][i+1]=b2;B[j+q][i+2]=b3;
                }
                for(int q=4;q<8;q++){
                    b1=A[i][j+q];b2=A[i+1][j+q];b3=A[i+2][j+q];
                    B[j+q][i]=b1;B[j+q][i+1]=b2;B[j+q][i+2]=b3;                    
                }           
            }
            else
            {
                for(int p=0;p<3;p++)
                    for(int q=0;q<5;q++)
                        B[q+j][p+i]=A[p+i][q+j];
            }
        }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
char trans_simple_desc1[] = "A simple transpose1";
void trans_simple1(int M, int N, int A[N][M], int B[M][N])
{
    int temp,temq;
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            for(int p=0;p<8;p++){
                for(int q=0;q<8;q++){
                    if(p==q){
                        temp=p;
                        temq=q;
                        continue;
                    }
                    B[8*j+q][8*i+p]=A[8*i+p][8*j+q];
                }
            B[8*j+temq][8*i+temp]=A[8*i+temp][8*j+temq];
            }
}
char trans_simple_desc2[] = "A simple transpose2";
void trans_simple2(int M, int N, int A[N][M], int B[M][N])
{
    int b1,b2,b3,b4,b5,b6,b7,b8;
    for(int i=0;i<N;i+=8)
        for(int j=0;j<M;j+=8)
        {
            for(int p=0;p<4;p++){
                b1=A[p+i][j];b2=A[p+i][j+1];b3=A[p+i][j+2];b4=A[p+i][j+3];b5=A[p+i][j+4];b6=A[p+i][j+5];b7=A[p+i][j+6];b8=A[p+i][j+7];
                B[j][p+i]=b1;B[j+1][p+i]=b2;B[j+2][p+i]=b3;B[j+3][p+i]=b4;B[j][p+i+4]=b5;B[j+1][p+i+4]=b6;B[j+2][p+i+4]=b7;B[j+3][p+i+4]=b8;
            }
            for(int p=0;p<4;p++){
                b1=A[i+4][j+p];b2=A[i+5][j+p];b3=A[i+6][j+p];b4=A[i+7][j+p];b5=B[j+p][i+4];b6=B[j+p][i+5];b7=B[j+p][i+6];b8=B[j+p][i+7];
                B[j+p][i+4]=b1;B[j+p][i+5]=b2;B[j+p][i+6]=b3;B[j+p][i+7]=b4;B[j+4+p][i]=b5;B[j+4+p][i+1]=b6;B[j+4+p][i+2]=b7;B[j+4+p][i+3]=b8;
            }
            for(int p=4;p<8;p++){
                b1=A[p+i][j+4];b2=A[p+i][j+5];b3=A[p+i][j+6];b4=A[p+i][j+7];
                B[j+4][p+i]=b1;B[j+5][p+i]=b2;B[j+6][p+i]=b3;B[j+7][p+i]=b4;
            }
        }
}
char trans_simple_desc3[] = "A simple transpose3";
void trans_simple3(int M, int N, int A[N][M], int B[M][N])
{
    int b1,b2,b3,b4,b5,b6,b7,b8;
    for(int i=0;i<N;i+=8)
        for(int j=0;j<M;j+=8)
        {
            if((i+8<N)&&(j+8<M))
            {
            for(int p=0;p<4;p++){
                b1=A[p+i][j];b2=A[p+i][j+1];b3=A[p+i][j+2];b4=A[p+i][j+3];b5=A[p+i][j+4];b6=A[p+i][j+5];b7=A[p+i][j+6];b8=A[p+i][j+7];
                B[j][p+i]=b1;B[j+1][p+i]=b2;B[j+2][p+i]=b3;B[j+3][p+i]=b4;B[j][p+i+4]=b5;B[j+1][p+i+4]=b6;B[j+2][p+i+4]=b7;B[j+3][p+i+4]=b8;
            }
            for(int p=0;p<4;p++){
                b1=A[i+4][j+p];b2=A[i+5][j+p];b3=A[i+6][j+p];b4=A[i+7][j+p];b5=B[j+p][i+4];b6=B[j+p][i+5];b7=B[j+p][i+6];b8=B[j+p][i+7];
                B[j+p][i+4]=b1;B[j+p][i+5]=b2;B[j+p][i+6]=b3;B[j+p][i+7]=b4;B[j+4+p][i]=b5;B[j+4+p][i+1]=b6;B[j+4+p][i+2]=b7;B[j+4+p][i+3]=b8;
            }
            for(int p=4;p<8;p++){
                b1=A[p+i][j+4];b2=A[p+i][j+5];b3=A[p+i][j+6];b4=A[p+i][j+7];
                B[j+4][p+i]=b1;B[j+5][p+i]=b2;B[j+6][p+i]=b3;B[j+7][p+i]=b4;
            }
            }
            else if((i+8<N)&&(j+8>=M))
            {
                for(int p=0;p<4;p++){
                    b1=A[i+p][j];b2=A[i+p][j+1];b3=A[i+p][j+2];b4=A[i+p][j+3];b5=A[i+p][j+4];
                    B[j][i+p]=b1;B[j+1][i+p]=b2;B[j+2][i+p]=b3;B[j+3][i+p]=b4;B[j+4][i+p]=b5;
                }
                for(int p=4;p<8;p++){
                    b1=A[i+p][j];b2=A[i+p][j+1];b3=A[i+p][j+2];b4=A[i+p][j+3];b5=A[i+p][j+4];
                    B[j][i+p]=b1;B[j+1][i+p]=b2;B[j+2][i+p]=b3;B[j+3][i+p]=b4;B[j+4][i+p]=b5;
                }
            }
            else if((i+8>=N)&&(j+8<M))
            {
                for(int q=0;q<4;q++){
                    b1=A[i][j+q];b2=A[i+1][j+q];b3=A[i+2][j+q];
                    B[j+q][i]=b1;B[j+q][i+1]=b2;B[j+q][i+2]=b3;
                }
                for(int q=4;q<8;q++){
                    b1=A[i][j+q];b2=A[i+1][j+q];b3=A[i+2][j+q];
                    B[j+q][i]=b1;B[j+q][i+1]=b2;B[j+q][i+2]=b3;                    
                }           
            }
            else
            {
                for(int p=0;p<3;p++)
                    for(int q=0;q<5;q++)
                        B[q+j][p+i]=A[p+i][q+j];
            }
        }
}
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
    //registerTransFunction(trans_simple1, trans_simple_desc1); 
    //registerTransFunction(trans_simple2, trans_simple_desc2); 
    //registerTransFunction(trans_simple3, trans_simple_desc3); 
    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

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

