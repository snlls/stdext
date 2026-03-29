#include <time.h>
#include <stdext/timespec.h>

static inline void timespec_math(struct timespec *ts, long count, long multiplier)
{
	long full_seconds = count / multiplier;
	ts->tv_sec += full_seconds;
	long rest = count - (full_seconds * multiplier);
	if(rest) {
		long tmp = ts->tv_nsec;
		tmp += (rest * (1000000000 / multiplier));
		if(count > 0) {
			if(tmp >= 1000000000) {
				ts->tv_sec++;
				tmp -= 1000000000;
			}
			ts->tv_nsec = tmp;
		}
		else {
			if(tmp < 0) {
				ts->tv_sec--;
				tmp += 1000000000;
			}
			ts->tv_nsec = tmp;
		}
	}
}

void timespec_add_ms(struct timespec *ts, long count) { timespec_math(ts, count, 1000); }

void timespec_add_us(struct timespec *ts, long count) { timespec_math(ts, count, 1000000); }

void timespec_sub_ms(struct timespec *ts, long count) { timespec_math(ts, -count, 1000); }

void timespec_sub_us(struct timespec *ts, long count) { timespec_math(ts, -count, 1000000); }

int timespec_cmp(const void *av, const void *bv)
{
	const struct timespec *a = av;
	const struct timespec *b = bv;
	
	if(a->tv_sec == b->tv_sec && a->tv_nsec == b->tv_nsec)
		return 0;
	if(a->tv_sec < b->tv_sec)
		return -1;
	if(a->tv_sec > b->tv_sec)
		return 1;

	if(a->tv_nsec < b->tv_nsec)
		return -1;

	return 1;
}

int timespec_cmp_descending(const void *a, const void *b)
{
	int ret = timespec_cmp(a, b);
	return ret * -1;
}


long timespec_diff_us(const struct timespec *after, const struct timespec *before)
{
	return ((long)after->tv_sec - (long)before->tv_sec) * (long)1000000 +
		((long)after->tv_nsec - (long)before->tv_nsec) / 1000;
}

long timespec_diff_ms(const struct timespec *after, const struct timespec *before)
{
	return ((long)after->tv_sec - (long)before->tv_sec) * (long)1000 +
		((long)after->tv_nsec - (long)before->tv_nsec) / 1000000;
}

struct timespec timespec_ms(time_t ms)
{
	struct timespec ts = {0};
	timespec_add_ms(&ts, ms);
	return ts;
}
struct timespec timespec_us(time_t us)
{
	struct timespec ts = {0};
	timespec_add_us(&ts, us);
	return ts;
}

struct timespec timespec_utc_ms(time_t ms)
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	timespec_add_ms(&ts, ms);
	return ts;
}

struct timespec timespec_utc_us(time_t us)
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	timespec_add_us(&ts, us);
	return ts;
}

struct timespec timespec_utc()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return ts;
}

void polling_timer_init(polling_timer *timer, unsigned delay)
{
	timespec_get(&timer->start, TIME_UTC);
	timer->delay_ms = delay;
}

polling_timer polling_timer_create(unsigned delay)
{
	polling_timer timer;
	polling_timer_init(&timer, delay);
	return timer;
}

bool polling_timer_ready(polling_timer *timer)
{
	struct timespec now = timespec_utc();
	long diff = timespec_diff_ms(&now, &timer->start);
	if(diff >= timer->delay_ms) {
		timer->start = now;
		return true;
	}
	return false;
}

void polling_timer_reset(polling_timer *timer)
{
	timer->start = timespec_utc();
}
