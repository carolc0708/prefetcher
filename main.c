#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include <xmmintrin.h>
#include <immintrin.h>//for AVX

#define TEST_W 4096
#define TEST_H 4096

/* provide the implementations of naive_transpose,
 * sse_transpose, sse_prefetch_transpose
 */

#include "impl.c"

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
    /* verify the result of 4x4 matrix */
    /*{
           int testin[16] = { 0, 1,  2,  3,  4,  5,  6,  7,
           8, 9, 10, 11, 12, 13, 14, 15
           };
           int testout[16];
           int expected[16] = { 0, 4,  8, 12, 1, 5,  9, 13,
           2, 6, 10, 14, 3, 7, 11, 15
           };

           for (int y = 0; y < 4; y++) {
           for (int x = 0; x < 4; x++)
           printf(" %2d", testin[y * 4 + x]);
           printf("\n");
           }
           printf("\n");
           sse_transpose(testin, testout, 4, 4);
           for (int y = 0; y < 4; y++) {
           for (int x = 0; x < 4; x++)
           printf(" %2d", testout[y * 4 + x]);
           printf("\n");
           }
           assert(0 == memcmp(testout, expected, 16 * sizeof(int)) &&
           "Verification fails");
    }*/

    {
        struct timespec start, end;
        int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        srand(time(NULL));
        for (int y = 0; y < TEST_H; y++)
            for (int x = 0; x < TEST_W; x++)
                *(src + y * TEST_W + x) = rand();

#ifdef AVX_PRE
        int *out0 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        clock_gettime(CLOCK_REALTIME, &start);
        avx_prefetch_transpose(src, out0, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        assert(0 == transpose_verify(src, out0, TEST_W, TEST_H) && "AVX_PRE Verification fails");
        //printf("avx prefetch: \t %ld us\n", diff_in_us(start, end));
        free(out0);
#endif

#ifdef AVX
        int *out1 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        clock_gettime(CLOCK_REALTIME, &start);
        avx_transpose(src, out1, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        assert(0 == transpose_verify(src, out1, TEST_W, TEST_H) && "AVX Verification fails");
        //printf("avx: \t %ld us\n", diff_in_us(start, end));
        free(out1);
#endif

#ifdef PRE
        int *out2 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        clock_gettime(CLOCK_REALTIME, &start);
        sse_prefetch_transpose(src, out2, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        assert(0 == transpose_verify(src, out2, TEST_W, TEST_H) && "PRE Verification fails");
        // printf("sse prefetch: \t %ld us\n", diff_in_us(start, end));
        free(out2);
#endif

#ifdef SSE
        int *out3 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        clock_gettime(CLOCK_REALTIME, &start);
        sse_transpose(src, out3, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        assert(0 == transpose_verify(src, out3, TEST_W, TEST_H) && "SSE Verification fails");
        // printf("sse: \t\t %ld us\n", diff_in_us(start, end));
        free(out3);
#endif

#ifdef NOR
        int *out4 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        clock_gettime(CLOCK_REALTIME, &start);
        naive_transpose(src, out4, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        // printf("naive: \t\t %ld us\n", diff_in_us(start, end));
        free(out4);
#endif
        free(src);
    }

    return 0;
}
