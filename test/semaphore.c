#include <stdext/semaphore.h>
#include <stdext/thread.h>
#include <stdext/timespec.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static semaphore sem = {0};
static semaphore startsem = {0};
static semaphore donesem = {0};
static atomic_int th_errors = 0;

#define COUNT 4

#define elog(fmt, ...)                                                                                                 \
	do {                                                                                                           \
		printf("%s:%d " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);                                          \
	} while(0)

static int semaphore_test_timeout_th(void *data)
{
	intptr_t index = (intptr_t)data;
	int err;
	semaphore_signal(&startsem, 1);
	if((err = semaphore_wait(&sem, 1)) != ETIMEDOUT) {
		elog("thread %ld did not timeout on semaphore err=%d", index, err);
		th_errors++;
	}
	semaphore_signal(&donesem, 1);
	return 0;
}

static int semaphore_test_timeout()
{
	int errors = 0;
	thrd_t ths[COUNT] = {0};
	int err;
	for(intptr_t n = 0; n < COUNT; n++) {
		int err = thrd_create(ths + n, semaphore_test_timeout_th, (void *)n);
		if(err) {
			elog("thrd_create failed %d %d", err, errno);
			exit(-1);
		}
	}
	for(intptr_t n = 0; n < COUNT; n++) {
		if((err = semaphore_wait(&startsem, 100))) {
			elog("waiting semaphore start failed %d index %ld", err, n);
			exit(-1);
		}
	}
	thrd_sleep_ms(100);
	for(intptr_t n = 0; n < COUNT; n++) {
		if((err = semaphore_wait(&donesem, 100))) {
			elog("waiting semaphore done failed %d index %ld\n start (%d %d) do (%d %d) done (%d %d)", err,
			     n, startsem.count, startsem.waiters, sem.count, sem.waiters, donesem.count,
			     donesem.waiters);
			exit(-1);
		}
	}
	int res;
	for(int n = 0; n < COUNT; n++) {
		int err = thrd_join(ths[n], &res);
		if(err) {
			elog("thrd_join failed %d %d", err, errno);
			exit(-1);
		}
	}
	return errors;
}

static int semaphore_test_signals_th(void *data)
{
	intptr_t index = (intptr_t)data;
	int err;
	semaphore_signal(&startsem, 1);
	if((err = semaphore_wait(&sem, 500)) != 0) {
		elog("thread %ld did not receive signal err=%d", index, err);
		th_errors++;
	}
	semaphore_signal(&donesem, 1);
	return 0;
}

static int semaphore_test_signals()
{
	int errors = 0;
	thrd_t ths[COUNT] = {0};
	int err;
	for(intptr_t n = 0; n < COUNT; n++) {
		int err = thrd_create(ths + n, semaphore_test_signals_th, (void *)n);
		if(err) {
			elog("fatal thread failed %d", err);
			exit(-1);
		}
	}
	for(intptr_t n = 0; n < COUNT; n++) {
		if((err = semaphore_wait(&startsem, 100))) {
			elog("waiting semaphore start failed %d index %d", err, (int)n);
			exit(-1);
		}
	}
	semaphore_signal(&sem, COUNT);
	for(int n = 0; n < COUNT; n++) {
		if((err = semaphore_wait(&donesem, 100))) {
			elog("waiting semaphore done failed %d index %d\nstart (%d %d) do (%d %d) done (%d %d)", err, n,
			     startsem.count, startsem.waiters, sem.count, sem.waiters, donesem.count, donesem.waiters);
			exit(-1);
		}
	}
	for(intptr_t n = 0; n < COUNT; n++) {
		int ret;
		thrd_join(ths[n], &ret);
	}

	if(startsem.count || sem.count || donesem.count) {
		elog("invalid counts on semaphore start %d do %d done %d", startsem.count, sem.count, donesem.count);
		exit(-1);
	}

	return 0;
}

int semaphore_test()
{
	int errors = 0;
	thrd_t ths[COUNT] = {0};
	int err;
	if((err = semaphore_init(&sem))) {
		elog("semaphore_init failed %d %s", err, strerror(err));
		exit(-1);
	}
	if((err = semaphore_init(&startsem))) {
		elog("semaphore_init failed %d %s", err, strerror(err));
		exit(-1);
	}
	if((err = semaphore_init(&donesem))) {
		elog("semaphore_init failed %d %s", err, strerror(err));
		exit(-1);
	}
	errors += semaphore_test_timeout();
	errors += semaphore_test_signals();
	if(errors) {
		elog("not even trying to go further");
		exit(-1);
	}

	semaphore_destroy(&startsem);
	semaphore_destroy(&donesem);
	semaphore_destroy(&sem);
	return errors + th_errors;
}
