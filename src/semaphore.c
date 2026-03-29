#include <stdext/semaphore.h>
#include <stdext/timespec.h>
#include <stdio.h>
int semaphore_wait(semaphore *sem, unsigned timeout_ms)
{
	if(!sem || !sem->active)
		return EINVAL;

	int err = 0;
	mtx_lock(&sem->mtx);
	if(sem->count) {
		sem->count--;
		goto done;
	}
	sem->waiters++;
	struct timespec ts = timespec_utc_ms(timeout_ms);
	err = cnd_timedwait(&sem->cond, &sem->mtx, &ts);
	sem->waiters--;
	if(err == thrd_timedout) {
		err = ETIMEDOUT;
		goto done;
	}
	else if(!err) {
		sem->count--;
		if(sem->count) {
			// let next know that they should wake up
			cnd_signal(&sem->cond);
		}
	}
	else {
		fprintf(stderr, "cnd_timedwait failed %d\n", err);
		err = ECANCELED;
	}
done:
	mtx_unlock(&sem->mtx);
	return err;
}

int semaphore_init(semaphore *sem)
{
	if(!sem)
		return EINVAL;
	if(sem->active)
		return EALREADY;
	mtx_init(&sem->mtx, mtx_timed);
	cnd_init(&sem->cond);
	sem->waiters = 0;
	sem->count = 0;
	sem->active = 1;
	return 0;
}

int semaphore_destroy(semaphore *sem)
{
	if(!sem)
		return EINVAL;
	if(!sem->active)
		return EALREADY;

	if(sem->waiters) {
		semaphore_broadcast(sem);
	}
	mtx_destroy(&sem->mtx);
	cnd_destroy(&sem->cond);
	sem->active = 0;
	return 0;
}

int semaphore_signal(semaphore *sem, unsigned count)
{
	if(!count)
		return 0;
	if(!sem || !sem->active)
		return EINVAL;

	mtx_lock(&sem->mtx);
	sem->count += count;
	cnd_signal(&sem->cond);
	mtx_unlock(&sem->mtx);
	return 0;
}

int semaphore_broadcast(semaphore *sem)
{
	if(!sem)
		return EINVAL;

	return semaphore_signal(sem, sem->waiters);
}
int semaphore_wait_all(semaphore *sem, unsigned timeout_ms)
{
	if(!sem)
		return EINVAL;
	if(!sem->active)
		return EALREADY;

	struct timespec start = timespec_utc();
	while(1) {
		struct timespec tmp = timespec_utc();
		long diff = timespec_diff_ms(&tmp, &start);
		if(sem->waiters && diff > 0) {
			semaphore_wait(sem, diff);
		}
	}
}
