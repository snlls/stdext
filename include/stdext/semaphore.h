#pragma once
#include <threads.h>
#include <stdatomic.h>
#include <errno.h>

/** @brief data for counting semaphore */
typedef struct {
	mtx_t mtx;
        cnd_t cond;
	atomic_uint count;
	atomic_uint waiters;
	atomic_bool active;
} semaphore;

/** @brief wait for signal on semaphore. If timeout_ms is 0, returns right away.
 * @param sem[in] instance
 * @param timeout_ms time to wait for signal
 * @return 0 if received signal, ETIMEOUT, EINVAL, ECANCELED
 */
int semaphore_wait(semaphore *sem, unsigned timeout_ms);

/** @brief wait all waiters for specified time. Mostly for closing things and debug.
 * @param sem[in] instance
 * @param timeout_ms time to wait in total
 * @return 0 if every waiter was done, ETIMEOUT, EINVAL, ECANCELED
 */
int semaphore_wait_all(semaphore *sem, unsigned timeout_ms);


/** @brief initialize semaphore
 * @param sem[in] instance
 * @return 0 on success, errno otherwise
 */
int semaphore_init(semaphore *sem);

/** @brief clear internal state, will broadcast if there are threads waiting this
 * @param sem[in] instance
 * @return 0 on success, EINVAL, EALREADY
 */ 
int semaphore_destroy(semaphore *sem);

/** @brief increment semaphore counter 
 * @param sem[in] instance
 * @param count 
 * @return 0 on success, EINVAL, EALREADY
 */ 
int semaphore_signal(semaphore *sem, unsigned count);

/** @brief signal the count of all waiting threads
 * @param sem[in] instance
 * @return 0 on success, EINVAL
 */ 
int semaphore_broadcast(semaphore *sem);
