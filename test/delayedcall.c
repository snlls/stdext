#include <stdext/delayedcall.h>
#include <stdext/thread.h>
#include <stdext/timespec.h>
#include <stdio.h>
#include <stdlib.h>

#define elog(fmt, ...)                                                                                                 \
	do {                                                                                                           \
		printf("%s:%d " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);                                          \
	} while(0)

static int delayedcall_errors = 0;
static int delayedcall_done = 0;
static struct timespec delayedcall_time = {0};

void delayedcall_test_fn(void *d)
{
	intptr_t v = (intptr_t)d;
	if(v != 1) {
		elog("user data was not passed to function");
		delayedcall_errors++;
	}
	delayedcall_done++;
}

static void delayedcall_do(int *errors)
{
	delayedcall_time = timespec_utc();
	delayedcall *dc = delayedcall_start(delayedcall_test_fn, (void *)1, 100, 0);
	if(!dc) {
		elog("delayed call init failed, tests stopped...");
		exit(-1);
	}
	thrd_sleep_ms(110);
	if(!delayedcall_done) {
		elog("delayed call did not finish in time");
		(*errors)++;
	}
	delayedcall_destroy(dc);
}

static void delayedcall_wait(int *errors)
{
	delayedcall_time = timespec_utc();
	int wait = 100;
	int limits = 2;
	delayedcall *dc = delayedcall_start(delayedcall_test_fn, (void *)1, wait, 0);
	if(!dc) {
		elog("delayed call init failed, tests stopped...");
		exit(-1);
	}
	delayedcall_wait_and_destroy(dc);
	struct timespec ts = timespec_utc();
	int diff = timespec_diff_ms(&ts, &delayedcall_time);
	if(diff < (wait - limits) || diff > (wait + limits)) {
		(*errors)++;
		elog("delayedcall did not run in limits");
	}
}

static void delayedcall_detach(int *errors, int wait)
{
	delayedcall_time = timespec_utc();
	delayedcall *dc = delayedcall_start(delayedcall_test_fn, (void *)1, wait, 1);
	if(!dc) {
		elog("delayed call init failed, tests stopped...");
		exit(-1);
	}
}

int delayedcall_test()
{
	int errors = 0;
	delayedcall_do(&errors);
	delayedcall_done = 0;
	delayedcall_wait(&errors);
	delayedcall_done = 0;
	int wait = 50;
	delayedcall_detach(&errors, wait);
	thrd_sleep_ms(wait + 2);
	if(!delayedcall_done) {
		elog("delayed call did not finish in time");
		errors++;
	}

	errors += delayedcall_errors;
	return errors;
}
