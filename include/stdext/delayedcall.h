#pragma once

#include <threads.h>
#include <stdint.h>
#include <stdbool.h>

/** @file
 * @brief Run delayed function in a new thread.
 * Spawning a new thread for a function is a bit of a overkill.
 * this is just a convenience when you don't want to setup thread pool.
 */

typedef void (*delayedcallfn)(void *);

/** opaque state */
typedef struct delayedcall delayedcall;

/** @brief Starts a new delayed call with the specified function, user data, timeout, and detach state.
 * @param fn The function to be called after the timeout.
 * @param user A pointer to user-defined data.
 * @param timeout_ms The timeout duration in milliseconds.
 * @param detach If true the thread is detached and it will clean itself. IF TRUE DO NOT CALL _destroy() on the result.
 * @return A pointer to the newly created delayed call structure or NULL
 */
delayedcall *delayedcall_start(delayedcallfn fn, void *user, unsigned timeout_ms, bool detach);

/** @brief signal cancel to delayedcall if it's still active. Clears the state and frees it.
 */
void delayedcall_destroy(delayedcall *delayedcall);

/** @brief wait for timeout, execute the function and then clears the delayedcall.
 */
void delayedcall_wait_and_destroy(delayedcall *delayedcall);

