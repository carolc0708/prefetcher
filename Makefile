CFLAGS = -msse2 -mavx2 --std gnu99 -O0 -Wall -pg

GIT_HOOKS := .git/hooks/pre-commit

EXEC = main main_sse main_pre main_avx main_avx_pre
all: $(EXEC)

main: $(GIT_HOOKS) main.c
	$(CC) $(CFLAGS) -DNOR -o main main.c

main_sse: $(GIT_HOOKS) main.c
	$(CC) $(CFLAGS) -DSSE -o main_sse main.c

main_pre: $(GIT_HOOKS) main.c
	$(CC) $(CFLAGS) -DPRE -o main_pre main.c

main_avx: $(GIT_HOOKS) main.c
	$(CC) $(CFLAGS) -DAVX -o main_avx main.c

main_avx_pre: $(GIT_HOOKS) main.c
	$(CC) $(CFLAGS) -DAVX_PRE -o main_avx_pre main.c

cache-test: $(EXEC)
	perf stat --repeat 100 \
                -e cache-misses,cache-references,instructions,cycles \
                ./main
	perf stat --repeat 100 \
                -e cache-misses,cache-references,instructions,cycles \
                ./main_sse
	perf stat --repeat 100 \
                -e cache-misses,cache-references,instructions,cycles \
                ./main_pre
	perf stat --repeat 100 \
                -e cache-misses,cache-references,instructions,cycles \
                ./main_avx
	perf stat --repeat 100 \
                -e cache-misses,cache-references,instructions,cycles \
                ./main_avx_pre


$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean:
	$(RM) main
