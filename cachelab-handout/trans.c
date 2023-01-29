/*
 * Zimo Meng 2100017702
 * 第一题分块8*8对对角线上和非对角线上的块分别处理 对角线上的块整行转置 非对角线上的块直接转置
 * 第二题分块8*8非对角线上的块分上下四行处理 对角线上的块要使用右边块的上四行作为临时存储
 * 第三题对前48*60的块进行16*4分块 对后20*60的块进行20*4分块
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
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    int i,j,k,l;
    int tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
    switch(M){
        case 32:
        for(i=0;i<32;i+=8){
            for(j=0;j<32;j+=8){
                if(i!=j){
                    for(k=0;k<8;++k){
                        for(l=0;l<8;++l){
                            tmp0 = A[i+k][j+l];
                            B[j+l][i+k] = tmp0;
                        }
                    }
                }
                else{
                    for(k=0;k<8;++k){
                        tmp0 = A[i+k][j+0];
                        tmp1 = A[i+k][j+1];
                        tmp2 = A[i+k][j+2];
                        tmp3 = A[i+k][j+3];
                        tmp4 = A[i+k][j+4];
                        tmp5 = A[i+k][j+5];
                        tmp6 = A[i+k][j+6];
                        tmp7 = A[i+k][j+7];
                        B[j+0][i+k]=tmp0;
                        B[j+1][i+k]=tmp1;
                        B[j+2][i+k]=tmp2;
                        B[j+3][i+k]=tmp3;
                        B[j+4][i+k]=tmp4;
                        B[j+5][i+k]=tmp5;
                        B[j+6][i+k]=tmp6;
                        B[j+7][i+k]=tmp7;
                    }
                }
            }
        }
        break;
        case 64:
        for(i=0;i<64;i+=8){
            for(j=0;j<64;j+=8){
                if(i==j&&i!=56){
                    for(k=0;k<4;++k){
                        tmp0 = A[i+k][j+0];
                        tmp1 = A[i+k][j+1];
                        tmp2 = A[i+k][j+2];
                        tmp3 = A[i+k][j+3];
                        tmp4 = A[i+k][j+4];
                        tmp5 = A[i+k][j+5];
                        tmp6 = A[i+k][j+6];
                        tmp7 = A[i+k][j+7];
                        B[j+0][i+8+k] = tmp0;
                        B[j+1][i+8+k] = tmp1;
                        B[j+2][i+8+k] = tmp2;
                        B[j+3][i+8+k] = tmp3;
                        B[j+0][i+8+k+4] = tmp4;
                        B[j+1][i+8+k+4] = tmp5;
                        B[j+2][i+8+k+4] = tmp6;
                        B[j+3][i+8+k+4] = tmp7;
                    }
                    for(k=4;k<8;++k){
                        tmp0 = A[i+k][j+0];
                        tmp1 = A[i+k][j+1];
                        tmp2 = A[i+k][j+2];
                        tmp3 = A[i+k][j+3];
                        tmp4 = A[i+k][j+4];
                        tmp5 = A[i+k][j+5];
                        tmp6 = A[i+k][j+6];
                        tmp7 = A[i+k][j+7];
                        B[i+k][j+0] = tmp0;
                        B[i+k][j+1] = tmp1;
                        B[i+k][j+2] = tmp2;
                        B[i+k][j+3] = tmp3;
                        B[i+k][j+4] = tmp4;
                        B[i+k][j+5] = tmp5;
                        B[i+k][j+6] = tmp6;
                        B[i+k][j+7] = tmp7;
                    }
                    for(k=0;k<4;++k){
                        for(l=k+1;l<4;++l){
                            tmp0 = B[i+4+k][j+l];
                            B[i+4+k][j+l] = B[i+4+l][j+k];
                            B[i+4+l][j+k] = tmp0;
                        }
                    }
                    for(k=0;k<4;++k){
                        for(l=k+1;l<4;++l){
                            tmp0 = B[i+4+k][j+4+l];
                            B[i+4+k][j+4+l] = B[i+4+l][j+4+k];
                            B[i+4+l][j+4+k] = tmp0;
                        }
                    }
                    for(k=0;k<4;++k){
                        for(l=0;l<4;++l){
                            tmp0 = B[i+4+k][j+l];
                            B[i+4+k][j+l] = B[i+k][j+12+l];
                            B[i+k][j+12+l] = tmp0;
                        }
                    }
                    for(k=0;k<4;++k){
                        for(l=0;l<8;++l){
                            tmp0 = B[i+k][j+8+l];
                            B[i+k][j+l] = tmp0;
                        }
                    }
                }
                else if(i==56&&j==56){
                    for(k=0;k<8;++k){
                        tmp0 = A[i+k][j+0];
                        tmp1 = A[i+k][j+1];
                        tmp2 = A[i+k][j+2];
                        tmp3 = A[i+k][j+3];
                        tmp4 = A[i+k][j+4];
                        tmp5 = A[i+k][j+5];
                        tmp6 = A[i+k][j+6];
                        tmp7 = A[i+k][j+7];
                        B[j+0][i+k]=tmp0;
                        B[j+1][i+k]=tmp1;
                        B[j+2][i+k]=tmp2;
                        B[j+3][i+k]=tmp3;
                        B[j+4][i+k]=tmp4;
                        B[j+5][i+k]=tmp5;
                        B[j+6][i+k]=tmp6;
                        B[j+7][i+k]=tmp7;
                    }
                }
                else{
                    for(k=0;k<4;++k){
                        tmp0 = A[i+k][j+0];
                        tmp1 = A[i+k][j+1];
                        tmp2 = A[i+k][j+2];
                        tmp3 = A[i+k][j+3];
                        tmp4 = A[i+k][j+4];
                        tmp5 = A[i+k][j+5];
                        tmp6 = A[i+k][j+6];
                        tmp7 = A[i+k][j+7];
                        B[j+0][i+k] = tmp0;
                        B[j+1][i+k] = tmp1;
                        B[j+2][i+k] = tmp2;
                        B[j+3][i+k] = tmp3;
                        B[j+0][i+k+4] = tmp4;
                        B[j+1][i+k+4] = tmp5;
                        B[j+2][i+k+4] = tmp6;
                        B[j+3][i+k+4] = tmp7;
                    }
                    for(l=0;l<4;++l){
                        tmp0 = B[j+l][i+4];
                        tmp1 = B[j+l][i+5];
                        tmp2 = B[j+l][i+6];
                        tmp3 = B[j+l][i+7];
                        tmp4 = A[i+4][j+l];
                        tmp5 = A[i+5][j+l];
                        tmp6 = A[i+6][j+l];
                        tmp7 = A[i+7][j+l];
                        B[j+l][i+4] = tmp4;
                        B[j+l][i+5] = tmp5;
                        B[j+l][i+6] = tmp6;
                        B[j+l][i+7] = tmp7;
                        tmp4 = A[i+4][j+l+4];
                        tmp5 = A[i+5][j+l+4];
                        tmp6 = A[i+6][j+l+4];
                        tmp7 = A[i+7][j+l+4];
                        B[j+l+4][i+0] = tmp0;
                        B[j+l+4][i+1] = tmp1;
                        B[j+l+4][i+2] = tmp2;
                        B[j+l+4][i+3] = tmp3;
                        B[j+l+4][i+4] = tmp4;
                        B[j+l+4][i+5] = tmp5;
                        B[j+l+4][i+6] = tmp6;
                        B[j+l+4][i+7] = tmp7;
                    }                    
                }
            }
        }
        break;
        case 60:
        for(i=0;i<68;i+=16){
            for(j=0;j<60;j+=4){
                for(k=i;k<i+16&&k<68;++k){
                    tmp0 = A[k][j+0];
                    tmp1 = A[k][j+1];
                    tmp2 = A[k][j+2];
                    tmp3 = A[k][j+3];
                    B[j+0][k] = tmp0;
                    B[j+1][k] = tmp1;
                    B[j+2][k] = tmp2;
                    B[j+3][k] = tmp3;
                }
            }
        }
        break;
    }
    ENSURES(is_transpose(M, N, A, B));
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

 /*
  * trans - A simple baseline transpose function, not optimized for the cache.
  */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
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

