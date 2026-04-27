#include <stdext/tpool.h>
#include <stdint.h>
#include <stdio.h>

#define elog(fmt, ...)                                                                                                 \
	do {                                                                                                           \
		printf("%s:%d " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);                                          \
	} while(0)

static atomic_int tpool_test_counter = 0;

static void *tpool_test_task(void *user, atomic_bool *closing)
{
	tpool_test_counter++;
	return NULL;
}

int tpool_test()
{
	int errors = 0;

	tpool *tp = tpool_create(4);
	if(!tp) {
		elog("tpool_create failed");
		errors++;
		return errors;
	}

#define COUNT 100
	tpoolid ids[COUNT];
	for(intptr_t n = 0; n < COUNT; n++) {
		ids[n] = tpool_add(tp, tpool_test_task, (void *)n, TPOOL_TASK_FLAG_NO_RESULT);
	}
	printf("waiting threads\n");
	tpool_wait_until_all_done(tp);
	printf("all tasks done\n");
	if(tpool_test_counter != COUNT) {
		elog("counter invalid %d expected %d", tpool_test_counter, COUNT);
		errors++;
	}
	tpool_destroy(tp);
	return errors;
}
