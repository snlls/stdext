#pragma once

#include <time.h>
#include <stdbool.h>

void timespec_add_ms(struct timespec *ts, long count);
void timespec_add_us(struct timespec *ts, long count);
void timespec_sub_ms(struct timespec *ts, long count);
void timespec_sub_us(struct timespec *ts, long count);

long timespec_diff_us(const struct timespec *after, const struct timespec *before);
long timespec_diff_ms(const struct timespec *after, const struct timespec *before);

/** @brief qsort cmp increasing order
 * @return 0 if a == b, -1 if a < b, 1 if a > b
 */
int timespec_cmp(const void *a, const void *b);
int timespec_cmp_descending(const void *a, const void *b);

/** @brief create new timespec from 0 + given milliseconds. */
struct timespec timespec_ms(time_t ms);
/** @brief create new timespec from 0 given microseconds. */
struct timespec timespec_us(time_t us);


/** @brief create new timespec from TIME_UTC + given milliseconds. */
struct timespec timespec_utc_ms(time_t ms);
/** @brief create new timespec from TIME_UTC + given microseconds. */
struct timespec timespec_utc_us(time_t us);
/** @brief create new timespec from TIME_UTC. */
struct timespec timespec_utc();

/** @brief struct to hold periodic polling timer, it doesn't do anything in itself. */
typedef struct {
    struct timespec start;
    unsigned delay_ms;
} polling_timer;

/** @brief initialize the struct */
void polling_timer_init(polling_timer *timer, unsigned delay);

/** @brief initialize timer to current time */
polling_timer polling_timer_create(unsigned delay);

/** @brief sets the time to current time if the timer is ready
 * @return true if the timer is ready and false otherwise
 */
bool polling_timer_ready(polling_timer *timer);

/** @brief set the timer to current time */
void polling_timer_reset(polling_timer *timer);


