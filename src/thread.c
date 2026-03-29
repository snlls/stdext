#include <stdext/thread.h>
#include <stdext/timespec.h>

int thrd_sleep_ms(unsigned ms)
{
	struct timespec ts = timespec_ms(ms);
	return thrd_sleep(&ts, NULL);
}
int thrd_sleep_us(unsigned us)
{
	struct timespec ts = timespec_ms(us);
	return thrd_sleep(&ts, NULL);
}
