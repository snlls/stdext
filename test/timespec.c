#include <ccpsx/timespec.h>
#include <stdio.h>
#include <ccpsx/array.h>
#include <stdlib.h>

#define SECNANO 1000000000L
#define MSNANO (SECNANO / 1000)
#define USNANO (SECNANO / 1000000)

#define elog(fmt, ...)							\
	do {								\
		printf("%s:%d " fmt "\n", __func__, __LINE__, __VA_ARGS__); \
		errors++;						\
	} while(0)

int timespec_test()
{

	int errors = 0;
	struct timespec ts;
	ts.tv_sec = 10;
	ts.tv_nsec = SECNANO / 2;
	long diff;
	int cmp;
	struct timespec tmp;

	tmp = ts;
	cmp = timespec_cmp(&tmp, &ts);
	if(cmp != 0) {
		elog("cmp %d", cmp);
	}

	timespec_add_ms(&tmp, 1500);
	timespec_add_ms(&tmp, 1);
	if(tmp.tv_sec != 12) {
		elog("add_ms %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	if(tmp.tv_nsec != MSNANO) {
		elog("add_ms %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	diff = timespec_diff_ms(&tmp, &ts);
	if(diff != 1501) {
		elog("diff %ld", diff);
	}

	tmp = ts;
	timespec_sub_ms(&tmp, 1500);
	timespec_sub_ms(&tmp, 1);
	if(tmp.tv_sec != 8) {
		elog("sub_ms %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	if(tmp.tv_nsec != SECNANO - MSNANO) {
		elog("sub_ms %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	diff = timespec_diff_ms(&tmp, &ts);
	if(diff != -1501) {
		elog("diff %ld", diff);
	}

	tmp = ts;

	timespec_add_us(&tmp, 1500000);
	timespec_add_us(&tmp, 1);
	if(tmp.tv_sec != 12) {
		elog("add_us %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	if(tmp.tv_nsec != USNANO) {
		elog("add_us %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	diff = timespec_diff_us(&tmp, &ts);
	if(diff != 1500001) {
		elog("diff %ld", diff);
	}

	tmp = ts;
	timespec_sub_us(&tmp, 1500000);
	timespec_sub_us(&tmp, 1);
	if(tmp.tv_sec != 8) {
		elog("sub_us %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	if(tmp.tv_nsec != SECNANO - USNANO) {
		elog("sub_us %ld %ld", tmp.tv_sec, tmp.tv_nsec);
	}
	diff = timespec_diff_us(&tmp, &ts);
	if(diff != -1500001) {
		elog("diff %ld", diff);
	}

	const int arrcount = 10;
	struct timespec specs[arrcount];
	for(int n = 0; n < arrcount; n++) {
		specs[n] = timespec_utc();
		specs[n].tv_nsec = 0;
		timespec_add_ms(specs + n, n * 1000);
	}
	array_shuffle(specs);
	qsort(specs, arrcount, sizeof(specs[0]), &timespec_cmp);
	for(int n = 0; n < arrcount - 1; n++) {
		if(specs[n + 1].tv_sec < specs[n].tv_sec) {
			errors++;
			elog("sort failed [%d - %d]", n, n + 1);
		}
	}
	return errors;
}
