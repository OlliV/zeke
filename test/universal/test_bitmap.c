/**
 * @file test_bitmap.c
 * @brief Test bitmap functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>
#include <punit.h>
#include <bitmap.h>

static void rnd_allocs(int n);
int unirand(int n);

static void setup()
{
}

static void teardown()
{
}

static char * test_alloc(void)
{
    bitmap_t bmap[64];
    size_t ret, err;

    memset(bmap, 0, sizeof(bmap));

    err = bitmap_block_alloc(&ret, 4, bmap, sizeof(bmap));
    pu_assert_equal("No error on allocation", err, 0);
    pu_assert_equal("4 bits allocated from bitmap", bmap[0], 0xf);

    return 0;
}

static void rnd_allocs(int n)
{
    const int allocs = 8000;
    const int maxsim = 1000;
    int i, j, err;
    size_t ret, sti = 0;
    size_t st[maxsim][2];
    bitmap_t bmap[n][E2BITMAP_SIZE(2048)];

    memset(bmap, 0, sizeof(bmap));

    for (i = 0; i < allocs; i++) {
        for (j = 0; j < n; j++) {
            if (unirand(1)) {
                while (!(st[sti][1] = unirand(100)));
                err = bitmap_block_alloc(&ret, st[sti][1], bmap[j], sizeof(bmap[j]));
                if (err == 0) {
                    st[sti][0] = ret;
                    sti++;
                } else if (sti > 0) {
                    sti--;
                    bitmap_block_update(bmap[j], 0, st[sti][0], st[sti][1]);
                }
            }

            if ((unirand(1) || sti >= maxsim) && (sti > 0)) {
                sti--;
                bitmap_block_update(bmap[j], 0, st[sti][0], st[sti][1]);
            }
        }
    }
}

static char * perf_test(void)
{
    const int trials = 3;
    int i, n;
    int result = 0;

    srand(time(NULL));

    printf("Performance test:\n");
    for (n = 1; n <= 4; n++) {
        result = 0;
        for (i = 0; i < trials; i++) {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            uint32_t start = tv.tv_sec * 1000 + tv.tv_usec / 1000;

            rnd_allocs(n);

            gettimeofday(&tv, NULL);
            uint32_t end = tv.tv_sec * 1000 + tv.tv_usec / 1000;
            result += end - start;
        }
        printf("\tn = %i: %i ms\n", n, result / trials);
    }

    return 0;
}

int unirand(int n)
{
    int partSize = (n == RAND_MAX) ? 1 : 1 + (RAND_MAX - n)/(n + 1);
    int maxUsefull = partSize * n + (partSize - 1);
    int draw;
    do {
        draw = rand();
    } while (draw > maxUsefull);

    return draw/partSize;
}

static void all_tests() {
    pu_def_test(perf_test, PU_RUN);
    pu_def_test(test_alloc, PU_RUN);
}

int main(int argc, char **argv)
{
    return pu_run_tests(&all_tests);
}
