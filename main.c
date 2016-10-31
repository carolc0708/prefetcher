#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#ifdef ARM
#include <arm_neon.h>
#include "impl_arm.c"
#else
#include <xmmintrin.h>
#include <immintrin.h>//for AVX
#include "impl.c"
#endif

#define TEST_W 4096
#define TEST_H 4096

/* provide the implementations of naive_transpose,
 * sse_transpose, sse_prefetch_transpose
 */


static long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}



int main(int argc, char *argv[])
{
    struct timespec start, end;
    int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    srand(time(NULL));
    for (int y = 0; y < TEST_H; y++)
        for (int x = 0; x < TEST_W; x++)
            *(src + y * TEST_W + x) = rand();
    int *out = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    clock_gettime(CLOCK_REALTIME, &start);
#ifdef AVX_PRE
    avx_prefetch_transpose(src, out, TEST_W, TEST_H);
    // assert(0 == transpose_verify(src, out0, TEST_W, TEST_H) && "AVX_PRE Verification fails");
    // printf("avx prefetch: \t %ld us\n", diff_in_us(start, end));
    //printf("avx prefetch per iteration: \t %lf us\n", (double)diff_in_us(start, end)/avx_iteration);
#endif
#ifdef AVX
    avx_transpose(src, out, TEST_W, TEST_H);
    //assert(0 == transpose_verify(src, out1, TEST_W, TEST_H) && "AVX Verification fails");
    // printf("avx: \t %ld us\n", diff_in_us(start, end));
    //printf("avx per iteration: \t %lf us\n", (double)diff_in_us(start, end)/avx_iteration);
#endif
#ifdef PRE
    sse_prefetch_transpose(src, out, TEST_W, TEST_H);
    //assert(0 == transpose_verify(src, out2, TEST_W, TEST_H) && "PRE Verification fails");
    //printf("sse prefetch: \t %ld us\n", diff_in_us(start, end));
    //printf("sse prefetch per iteration: \t %lf us\n", (double)diff_in_us(start, end)/sse_iteration);
#endif
#ifdef SSE
    sse_transpose(src, out, TEST_W, TEST_H);
    //assert(0 == transpose_verify(src, out3, TEST_W, TEST_H) && "SSE Verification fails");
    //printf("sse: \t\t %ld us\n", diff_in_us(start, end));
    //printf("sse per iteration: \t\t %lf us\n", (double)diff_in_us(start, end)/sse_iteration);
#endif
#ifdef NOR
    naive_transpose(src, out, TEST_W, TEST_H);
    //printf("naive: \t\t %ld us\n", diff_in_us(start, end));
    //printf("naive per iteration: \t\t %lf us\n", (double)diff_in_us(start, end)/naive_iteration);
#endif
#ifdef ARM
    neon_transpose(src, out, TEST_W, TEST_H);
    if(!transpose_verify(src, out5, TEST_W, TEST_H)) printf("NEON verify fails;");
    //printf("neon: \t\t %ld us\n", diff_in_us(start, end));
    //printf("sse per iteration: \t\t %lf us\n", (double)diff_in_us(start, end)/sse_iteration);
#endif
    clock_gettime(CLOCK_REALTIME, &end);
    //free all used memory
    free(out);
    free(src);
    return 0;
}
