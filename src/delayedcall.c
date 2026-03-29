#include <stdext/delayedcall.h>
#include <stdext/timespec.h>
#include <stdlib.h>

struct delayedcall {
	unsigned timeout_ms;
	thrd_t th;
	cnd_t cnd;
	mtx_t mtx;
	void *user;
	delayedcallfn fn;
	bool done;
	bool detached;
	void *this;
};

static int delayedcall_process(void *user)
{
	delayedcall *tc = user;
	struct timespec ts = timespec_utc_ms(tc->timeout_ms);
	int ret = cnd_timedwait(&tc->cnd, &tc->mtx, &ts);
	// if there was cancel signal, don't execute the function
	if(ret == thrd_timedout) {
		tc->fn(tc->user);
	}
	if(tc->detached) {
		cnd_destroy(&tc->cnd);
		mtx_destroy(&tc->mtx);
		free(tc);
	}
	else {
		tc->done = 1;
	}
	return 0;
}

delayedcall *delayedcall_start(delayedcallfn fn, void *user, unsigned timeout, bool detach)
{
	delayedcall *cb = calloc(1, sizeof(*cb));
	if(!cb)
		return 0;
	cb->fn = fn;
	cb->user = user;
	cb->detached = detach;
	cb->timeout_ms = timeout;
	int err;
	if((err = cnd_init(&cb->cnd)) != thrd_success) {
		return NULL;
	}
	if((err = mtx_init(&cb->mtx, mtx_plain)) != thrd_success) {
		cnd_destroy(&cb->cnd);
		return NULL;
	}
	if((err = thrd_create(&cb->th, delayedcall_process, cb))) {
		cnd_destroy(&cb->cnd);
		mtx_destroy(&cb->mtx);
		free(cb);
		return NULL;
	}
	if(detach) {
		thrd_detach(cb->th);
	}
	return cb;
}

static void delayedcall_cancel(delayedcall *delayedcall)
{
	if(!delayedcall)
		return;
	if(delayedcall->done) {
		thrd_join(delayedcall->th, NULL);
		return;
	}
	cnd_signal(&delayedcall->cnd);
	thrd_join(delayedcall->th, NULL);
}

void delayedcall_wait_and_destroy(delayedcall *delayedcall)
{
	if(!delayedcall->done) {
		thrd_join(delayedcall->th, NULL);
	}
	mtx_destroy(&delayedcall->mtx);
	cnd_destroy(&delayedcall->cnd);
	free(delayedcall);
}

void delayedcall_destroy(delayedcall *delayedcall)
{
	delayedcall_cancel(delayedcall);
	mtx_destroy(&delayedcall->mtx);
	cnd_destroy(&delayedcall->cnd);
	free(delayedcall);
}
