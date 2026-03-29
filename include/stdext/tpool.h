#pragma once
#include <stdext/semaphore.h>

#define TPOOLID_INVALID 0

typedef struct se_tpool se_tpool;

typedef enum {
	TPOOL_TASK_STATE_INIT = 0,
	TPOOL_TASK_STATE_ADDED,
	TPOOL_TASK_STATE_QUEUE,
	TPOOL_TASK_STATE_RUNNING,
	TPOOL_TASK_STATE_DONE,
} tpool_task_state;

typedef enum {
	TPOOL_TASK_FLAG_NONE = (0 << 0),
	/** @brief if you don't care about the result,
	 * this flag will clear the task right away after running it
	 */
	TPOOL_TASK_FLAG_NO_RESULT = (1 << 0),
} tpool_task_flag;

typedef void *(tpool_task_function)(void *userdata, atomic_bool *closing);

typedef struct tpool_task tpool_task;

typedef struct tpool tpool;
typedef unsigned tpoolid;

/** @brief create thread pool
 * @param threads count of threads to create, if 0 use system thread count
 * @return pointer to instance or NULL on error
 */
tpool *tpool_create(unsigned threads);


 /**
 * Adds a new task to the thread pool.
 *
 * @param tp The thread pool to which the task will be added.
 * @param function The function that will be executed by the task.
 * @param userdata A pointer to the data that will be passed to the function.
 * @return task id, INVALID id on error
 */
tpoolid tpool_add(tpool *tp, tpool_task_function function, void *userdata, unsigned flags);

/**
 * @brief count of tasks either running or queued
 * @param tp Pointer to the thread pool structure.
 */
unsigned tpool_task_count(tpool *tp);

/**
 * Waits for a task to complete and clean it
 *
 * @param tp Pointer to the thread pool.
 * @return task result or NULL on error
 */
void *tpool_wait(tpool *tp, tpoolid id);

/**
 * get task result and clean the task
 *
 * @param tp Pointer to the thread pool.
 * @return task result or NULL on error
 */
void *tpool_result(tpool *tp, tpoolid id);


 /**
 * @brief Waits for all tasks in the thread pool to complete and then destroys the thread pool.
 * 
 * This function ensures that all tasks currently in the queue are executed and waits for all worker 
 * threads to finish before destroying the thread pool. It is a blocking call that waits for the 
 * thread pool to be completely shut down.
 * 
 * @param tp Pointer to the thread pool structure that needs to be destroyed.
 */
void tpool_wait_and_destroy(tpool *tp);


 /**
 * @brief Destroys the thread pool, freeing all associated resources.
 * 
 * This function terminates all worker threads and skips the functions if possible.
 * It then frees internal state.
 */
void tpool_destroy(tpool *tp);

