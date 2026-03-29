#include <stdext/memory.h>
#include <stdext/semaphore.h>
#include <stdext/tpool.h>
#include <stdext/vector.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>

struct tpool_task {
	_Atomic tpool_task_state state;
	void *result;
	void *user;
	tpool_task_function *function;
	tpoolid id;
	unsigned flags;
};

struct tpool {

	semaphore dosem;
	semaphore donesem;
	vector(struct tpool_task) tasks;
	mtx_t lock;
	thrd_t *threads;
	unsigned threads_count;
	atomic_bool closing;
	tpoolid id;
	unsigned count;
};

typedef struct {
	bool done;
	tpool_task_function *function;
} tpool_worker;

static void tpool_task_clear_id(tpool *tp, tpoolid id)
{
	mtx_lock(&tp->lock);
	for(int n = 0; n < vector_length(&tp->tasks); n++) {		
		if(vector_get(&tp->tasks, n)->id == id) {
			vector_remove(&tp->tasks, n);
			break;
		}
		
	}

	mtx_unlock(&tp->lock);
}

static int tpool_run(void *ptr)
{
	tpool *tp = ptr;
	while(!tp->closing) {
		mtx_lock(&tp->lock);
		int len = vector_length(&tp->tasks);
		int index = -1;
		tpool_task task;
		for(int n = 0; n < len; n++) {
			if(vector_get(&tp->tasks, n)->state == TPOOL_TASK_STATE_QUEUE) {
				vector_get(&tp->tasks, n)->state = TPOOL_TASK_STATE_RUNNING;
				index = n;
				task = *vector_get(&tp->tasks, n);
				break;
			}
		}
		mtx_unlock(&tp->lock);
		if(index >= 0) {
			void *result = task.function(task.user, &tp->closing);
			if(vector_get(&tp->tasks, index)->flags & TPOOL_TASK_FLAG_NO_RESULT) {
				tpool_task_clear_id(tp, task.id);
			}
			else {
				
				mtx_lock(&tp->lock);
				for(int n = 0; n < vector_length(&tp->tasks); n++) {
					tpool_task *tmp = vector_get(&tp->tasks, n);
					if(tmp->id == task.id) {
						tmp->result = result;
						break;
					}
				}
				mtx_unlock(&tp->lock);
			}
			semaphore_signal(&tp->donesem, 1);
			continue;
		}
		semaphore_wait(&tp->dosem, 5000);
	}
	return 0;
}

/** brief returns at least one if could not get the value */
static int system_native_threads()
{
	long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
	if(number_of_processors <= 0) {
		return 1;
	}
	return number_of_processors;
}


tpool *tpool_create(unsigned threads)
{
	unsigned ths_count = 1;
	if(threads <= 0) {
		ths_count = system_native_threads();
	} 
	else {
		ths_count = threads;
	}
	
	tpool *tp = ext_alloc(sizeof(*tp) + ths_count * sizeof(thrd_t));
	if(!tp)
		return NULL;
	tp->threads_count = ths_count;
	tp->threads = (thrd_t *)(tp + 1);

	semaphore_init(&tp->dosem);
	semaphore_init(&tp->donesem);
	int ret;
	if((ret = mtx_init(&tp->lock, mtx_plain))) {
		return NULL;
	}

	tp->id = 1;
	for(unsigned n = 0; n < ths_count; n++) {
		thrd_create(tp->threads + n, tpool_run, tp);
	}

	return tp;
}

tpoolid tpool_add(tpool *tp, tpool_task_function function, void *userdata, unsigned flags)
{
	if(!tp || tp->closing)
		return TPOOLID_INVALID;
	tpoolid ret = TPOOLID_INVALID;
	mtx_lock(&tp->lock);
	tpool_task task;
	task.flags = flags;
	task.function = function;
	task.state = TPOOL_TASK_STATE_QUEUE;
	ret = tp->id++;
	task.id = ret;
	if(ret == TPOOLID_INVALID)
		goto end;

	task.result = userdata;
	tpool_task *tmp = vector_add(&tp->tasks);
	*tmp = task;
end:
	mtx_unlock(&tp->lock);
	semaphore_signal(&tp->dosem, 1);
	return ret;
}

unsigned tpool_task_count(tpool *tp)
{
	if(!tp)
		return 0;
	unsigned ret = 0;
	mtx_lock(&tp->lock);
	for(int n = 0; n < vector_length(&tp->tasks); n++) {
		int state = vector_get(&tp->tasks, n)->state;
		if(state == TPOOL_TASK_STATE_QUEUE || state == TPOOL_TASK_STATE_RUNNING) {
			ret++;
		}
	}
	mtx_unlock(&tp->lock);
	return ret;
}

void *tpool_wait(tpool *tp, tpoolid id)
{
	if(!tp || id == TPOOLID_INVALID)
		return NULL;
	void *ret;
	bool done = false;
	while(!done) {
		bool wait = false;
		int n;
		mtx_lock(&tp->lock);
		for(n = 0; n < vector_length(&tp->tasks); n++) {
			tpool_task *tmp = vector_get(&tp->tasks, n);
			if(tmp->id == id) {
				if(tmp->state == TPOOL_TASK_STATE_DONE) {
					ret = tmp->result;
					done = true;
				}
				else {
					wait = true;
				}
				break;
			}
		}
		mtx_unlock(&tp->lock);
		if(!done && !wait) {
			// no task with this id?
			return NULL;
		}
		if(wait)
			semaphore_wait(&tp->donesem, 2000);
		else if(done)
			tpool_task_clear_id(tp, id);
	}
	return ret;
}

void tpool_wait_and_destroy(tpool *tp)
{
	if(!tp || tp->closing)
		return;
	tp->closing = true;
	bool done = false;
	while(!done) {
		unsigned count = 0;
		tpoolid id;
		mtx_lock(&tp->lock);
		count = vector_length(&tp->tasks);
		if(count) {
			id = vector_get(&tp->tasks, 0)->id;
		}
		mtx_unlock(&tp->lock);
		if(!count) {
			break;
		}
		tpool_wait(tp, id);
	}
	tpool_destroy(tp);
}

void tpool_destroy(tpool *tp)
{
	if(!tp)
		return;

	tp->closing = true;
	for(unsigned n = 0; n < tp->threads_count; n++) {
		int ret;
		semaphore_broadcast(&tp->dosem);
		thrd_join(tp->threads[n], &ret);
	}
	semaphore_destroy(&tp->donesem);
	semaphore_destroy(&tp->dosem);
	mtx_destroy(&tp->lock);
	vector_free(&tp->tasks);
	free(tp);
}
