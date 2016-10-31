CFLAGS = -msse2 -mavx2 --std gnu99 -O0 -Wall -pg
ARM_CC ?= arm-linux-gnueabihf-gcc-5
ARM_CFLAGS = -c -g -Wall -Wextra -Ofast -mfpu=neon
ARM_LDFLAGS = -Wall -g -Wextra -Ofast
GIT_HOOKS := .git/hooks/pre-commit

EXEC = main main_sse main_pre main_avx main_avx_pre main_arm
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

main_arm: $(GIT_HOOKS) main.c
	$(ARM_CC) $(ARM_CFLAGS) -DARM -o main_arm.o main.c
	$(ARM_CC) $(ARM_LDFLAGS) -o main_arm main_arm.o
main_arm_pre: $(GIT_HOOKS) main.c
	$(ARM_CC) $(ARM_CFLAGS) -DARM_PRE -o main_arm.o main.c
	$(ARM_CC) $(ARM_LDFLAGS) -o main_arm_pre main_arm.o

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
	$(RM) main main_arm main_avx main_avx_pre main_pre main_sse main_arm_pre
