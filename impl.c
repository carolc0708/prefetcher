#ifndef TRANSPOSE_IMPL
#define TRANSPOSE_IMPL
long naive_iteration;
long sse_iteration;
long avx_iteration;

/*
//verify
int transpose_verify(int *test_src, int *test_dest, int w, int h)
{
	int *expected  = (int *) malloc(sizeof(int) * w * h);
	naive_transpose(test_src, expected, w, h);
	if(memcmp(test_dest, expected, w*h*sizeof(int)) != 0) {
		return 1;
	} else {
		return 0;
	}
}
#ifndef ARM
void naive_transpose(int *src, int *dst, int w, int h)
{
	naive_iteration = 0;
	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
		{ *(dst + x * h + y) = *(src + y * w + x); naive_iteration +=1;}
}

void sse_transpose(int *src, int *dst, int w, int h)
{
	sse_iteration = 0;
	for (int x = 0; x < w; x += 4) {
		for (int y = 0; y < h; y += 4) {
			__m128i I0 = _mm_loadu_si128((__m128i *)(src + (y + 0) * w + x));
			__m128i I1 = _mm_loadu_si128((__m128i *)(src + (y + 1) * w + x));
			__m128i I2 = _mm_loadu_si128((__m128i *)(src + (y + 2) * w + x));
			__m128i I3 = _mm_loadu_si128((__m128i *)(src + (y + 3) * w + x));
			__m128i T0 = _mm_unpacklo_epi32(I0, I1);
			__m128i T1 = _mm_unpacklo_epi32(I2, I3);
			__m128i T2 = _mm_unpackhi_epi32(I0, I1);
			__m128i T3 = _mm_unpackhi_epi32(I2, I3);
			I0 = _mm_unpacklo_epi64(T0, T1);
			I1 = _mm_unpackhi_epi64(T0, T1);
			I2 = _mm_unpacklo_epi64(T2, T3);
			I3 = _mm_unpackhi_epi64(T2, T3);
			_mm_storeu_si128((__m128i *)(dst + ((x + 0) * h) + y), I0);
			_mm_storeu_si128((__m128i *)(dst + ((x + 1) * h) + y), I1);
			_mm_storeu_si128((__m128i *)(dst + ((x + 2) * h) + y), I2);
			_mm_storeu_si128((__m128i *)(dst + ((x + 3) * h) + y), I3);
			sse_iteration +=1;
		}
	}
}

void sse_prefetch_transpose(int *src, int *dst, int w, int h)
{
	sse_iteration = 0;
	for (int x = 0; x < w; x += 4) {
		for (int y = 0; y < h; y += 4) {
#define PFDIST  8
			_mm_prefetch(src+(y + PFDIST + 0) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 1) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 2) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 3) *w + x, _MM_HINT_T1);

			__m128i I0 = _mm_loadu_si128 ((__m128i *)(src + (y + 0) * w + x));
			__m128i I1 = _mm_loadu_si128 ((__m128i *)(src + (y + 1) * w + x));
			__m128i I2 = _mm_loadu_si128 ((__m128i *)(src + (y + 2) * w + x));
			__m128i I3 = _mm_loadu_si128 ((__m128i *)(src + (y + 3) * w + x));
			__m128i T0 = _mm_unpacklo_epi32(I0, I1);
			__m128i T1 = _mm_unpacklo_epi32(I2, I3);
			__m128i T2 = _mm_unpackhi_epi32(I0, I1);
			__m128i T3 = _mm_unpackhi_epi32(I2, I3);
			I0 = _mm_unpacklo_epi64(T0, T1);
			I1 = _mm_unpackhi_epi64(T0, T1);
			I2 = _mm_unpacklo_epi64(T2, T3);
			I3 = _mm_unpackhi_epi64(T2, T3);
			_mm_storeu_si128((__m128i *)(dst + ((x + 0) * h) + y), I0);
			_mm_storeu_si128((__m128i *)(dst + ((x + 1) * h) + y), I1);
			_mm_storeu_si128((__m128i *)(dst + ((x + 2) * h) + y), I2);
			_mm_storeu_si128((__m128i *)(dst + ((x + 3) * h) + y), I3);
			sse_iteration += 1;
		}
	}
}

//avx_transpose
void avx_transpose(int *src, int *dst, int w, int h)
{
	avx_iteration = 0;
	for ( int x = 0; x < w; x += 8 ) {
		for ( int y = 0; y < h; y += 8 ) {
			__m256i I0 = _mm256_loadu_si256((__m256i *)(src + (y + 0) * w + x));
			__m256i I1 = _mm256_loadu_si256((__m256i *)(src + (y + 1) * w + x));
			__m256i I2 = _mm256_loadu_si256((__m256i *)(src + (y + 2) * w + x));
			__m256i I3 = _mm256_loadu_si256((__m256i *)(src + (y + 3) * w + x));
			__m256i I4 = _mm256_loadu_si256((__m256i *)(src + (y + 4) * w + x));
			__m256i I5 = _mm256_loadu_si256((__m256i *)(src + (y + 5) * w + x));
			__m256i I6 = _mm256_loadu_si256((__m256i *)(src + (y + 6) * w + x));
			__m256i I7 = _mm256_loadu_si256((__m256i *)(src + (y + 7) * w + x));

			__m256i T0 = _mm256_unpacklo_epi32(I0, I1);
			__m256i T1 = _mm256_unpackhi_epi32(I0, I1);
			__m256i T2 = _mm256_unpacklo_epi32(I2, I3);
			__m256i T3 = _mm256_unpackhi_epi32(I2, I3);
			__m256i T4 = _mm256_unpacklo_epi32(I4, I5);
			__m256i T5 = _mm256_unpackhi_epi32(I4, I5);
			__m256i T6 = _mm256_unpacklo_epi32(I6, I7);
			__m256i T7 = _mm256_unpackhi_epi32(I6, I7);

			I0 = _mm256_unpacklo_epi64(T0, T2);
			I1 = _mm256_unpackhi_epi64(T0, T2);
			I2 = _mm256_unpacklo_epi64(T1, T3);
			I3 = _mm256_unpackhi_epi64(T1, T3);
			I4 = _mm256_unpacklo_epi64(T4, T6);
			I5 = _mm256_unpackhi_epi64(T4, T6);
			I6 = _mm256_unpacklo_epi64(T5, T7);
			I7 = _mm256_unpackhi_epi64(T5, T7);

			T0 = _mm256_permute2x128_si256(I0, I4, 0x20);
			T1 = _mm256_permute2x128_si256(I1, I5, 0x20);
			T2 = _mm256_permute2x128_si256(I2, I6, 0x20);
			T3 = _mm256_permute2x128_si256(I3, I7, 0x20);
			T4 = _mm256_permute2x128_si256(I0, I4, 0x31);
			T5 = _mm256_permute2x128_si256(I1, I5, 0x31);
			T6 = _mm256_permute2x128_si256(I2, I6, 0x31);
			T7 = _mm256_permute2x128_si256(I3, I7, 0x31);

			_mm256_storeu_si256((__m256i *)(dst + ((x + 0) * h) + y), T0);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 1) * h) + y), T1);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 2) * h) + y), T2);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 3) * h) + y), T3);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 4) * h) + y), T4);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 5) * h) + y), T5);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 6) * h) + y), T6);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 7) * h) + y), T7);
			avx_iteration +=1;
		}
	}
}

//avx_prefetch_transpose
void avx_prefetch_transpose(int *src, int *dst, int w, int h)
{
	avx_iteration = 0;
	for (int x = 0; x < w; x += 8) {
		for (int y = 0; y < h; y += 8) {
#define PFDIST 8
			_mm_prefetch(src+(y + PFDIST + 0) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 1) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 2) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 3) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 4) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 5) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 6) *w + x, _MM_HINT_T1);
			_mm_prefetch(src+(y + PFDIST + 7) *w + x, _MM_HINT_T1);

			__m256i I0 = _mm256_loadu_si256((__m256i *)(src + (y + 0) * w + x));
			__m256i I1 = _mm256_loadu_si256((__m256i *)(src + (y + 1) * w + x));
			__m256i I2 = _mm256_loadu_si256((__m256i *)(src + (y + 2) * w + x));
			__m256i I3 = _mm256_loadu_si256((__m256i *)(src + (y + 3) * w + x));
			__m256i I4 = _mm256_loadu_si256((__m256i *)(src + (y + 4) * w + x));
			__m256i I5 = _mm256_loadu_si256((__m256i *)(src + (y + 5) * w + x));
			__m256i I6 = _mm256_loadu_si256((__m256i *)(src + (y + 6) * w + x));
			__m256i I7 = _mm256_loadu_si256((__m256i *)(src + (y + 7) * w + x));

			__m256i T0 = _mm256_unpacklo_epi32(I0, I1);
			__m256i T1 = _mm256_unpackhi_epi32(I0, I1);
			__m256i T2 = _mm256_unpacklo_epi32(I2, I3);
			__m256i T3 = _mm256_unpackhi_epi32(I2, I3);
			__m256i T4 = _mm256_unpacklo_epi32(I4, I5);
			__m256i T5 = _mm256_unpackhi_epi32(I4, I5);
			__m256i T6 = _mm256_unpacklo_epi32(I6, I7);
			__m256i T7 = _mm256_unpackhi_epi32(I6, I7);

			I0 = _mm256_unpacklo_epi64(T0, T2);
			I1 = _mm256_unpackhi_epi64(T0, T2);
			I2 = _mm256_unpacklo_epi64(T1, T3);
			I3 = _mm256_unpackhi_epi64(T1, T3);
			I4 = _mm256_unpacklo_epi64(T4, T6);
			I5 = _mm256_unpackhi_epi64(T4, T6);
			I6 = _mm256_unpacklo_epi64(T5, T7);
			I7 = _mm256_unpackhi_epi64(T5, T7);

			T0 = _mm256_permute2x128_si256(I0, I4, 0x20);
			T1 = _mm256_permute2x128_si256(I1, I5, 0x20);
			T2 = _mm256_permute2x128_si256(I2, I6, 0x20);
			T3 = _mm256_permute2x128_si256(I3, I7, 0x20);
			T4 = _mm256_permute2x128_si256(I0, I4, 0x31);
			T5 = _mm256_permute2x128_si256(I1, I5, 0x31);
			T6 = _mm256_permute2x128_si256(I2, I6, 0x31);
			T7 = _mm256_permute2x128_si256(I3, I7, 0x31);

			_mm256_storeu_si256((__m256i *)(dst + ((x + 0) * h) + y), T0);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 1) * h) + y), T1);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 2) * h) + y), T2);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 3) * h) + y), T3);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 4) * h) + y), T4);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 5) * h) + y), T5);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 6) * h) + y), T6);
			_mm256_storeu_si256((__m256i *)(dst + ((x + 7) * h) + y), T7);
			avx_iteration += 1;
		}
	}
}
#endif
*/
void neon_transpose(int *src, int *dst, int w, int h)
{
    sse_iteration = 0;
    for (int x = 0; x < w; x += 4) {
        for(int y = 0; x < h; y += 4) {

            int32x4_t I0 = vld1q_s32((int32_t *)(src + (y + 0) * w + x));
            int32x4_t I1 = vld1q_s32((int32_t *)(src + (y + 1) * w + x));
            int32x4_t I2 = vld1q_s32((int32_t *)(src + (y + 2) * w + x));
            int32x4_t I3 = vld1q_s32((int32_t *)(src + (y + 3) * w + x));

            vzipq_s32(I0 , I1);//I0: T0, I1:T2
            vzipq_s32(I2 , I3);//I2: T1, I3:T3

            int32x4_t T0 = vcombine_s32(vget_low_s32(I0), vget_low_s32(I1));//vcombine_s32(low,high)
            int32x4_t T1 = vcombine_s32(vget_high_s32(I0), vget_high_s32(I1));
            int32x4_t T2 = vcombine_s32(vget_low_s32(I2), vget_low_s32(I3));
            int32x4_t T3 = vcombine_s32(vget_high_s32(I2), vget_high_s32(I3));

            vst1q_s32((int32_t *)(dst + ((x + 0) * h) + y) , T0);
            vst1q_s32((int32_t *)(dst + ((x + 1) * h) + y) , T1);
            vst1q_s32((int32_t *)(dst + ((x + 2) * h) + y) , T2);
            vst1q_s32((int32_t *)(dst + ((x + 3) * h) + y) , T3);

        }
    }
}
#endif /* TRANSPOSE_IMPL */
