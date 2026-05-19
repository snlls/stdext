#pragma once

#include <stdlib.h>
#include <threads.h>


extern void (*ext_memory_fault_handler)();

/** @brief calloc memory, if this fails it calls error handler, default handler aborts. */
static inline void *ext_alloc(size_t size)
{
	void *p = calloc(1, size);
	if(!p) {
		if(ext_memory_fault_handler) {
			ext_memory_fault_handler();
		}
	}
	return p;
}

#define ext_new(var) ext_alloc(sizeof(* var))


#ifdef NDEBUG

#define ALLOCATOR_PROT
#define ALLOCATOR_ARGS
#define ALLOCATOR_ARGS_CALL

#else

#define ALLOCATOR_ADD_LINES 1

#define ALLOCATOR_PROT , const char *file, int line, const char *func
#define ALLOCATOR_ARGS ,__FILE__, __LINE__, __func__
#define ALLOCATOR_ARGS_CALL ,file, line, func

#endif


typedef struct allocator allocator;
/** @brief interface for a allocator implementation  */
struct allocator {
	void *(*alloc)(allocator *a, size_t ALLOCATOR_PROT);
	void *(*resize)(allocator *a, void*, size_t ALLOCATOR_PROT);
	void (*free)(allocator *a, void* ALLOCATOR_PROT);
	void (*fail)(allocator *a ALLOCATOR_PROT);
	void (*reset)(allocator *a);
	char *(*debug)(allocator *a, char *buf, int buf_len);
	void *data;
	mtx_t lock;
};

// generic calling methods, when debugging these will catch the line and function where the allocation happened

#define allocator_alloc(a, size) (a)->alloc(a, (size) ALLOCATOR_ARGS)
#define allocator_resize(a, p, size) (a)->resize((a), (p), (size) ALLOCATOR_ARGS)
#define allocator_free(a, p) (a)->free((a), (p) ALLOCATOR_ARGS)
#define allocator_reset(a) do { if((a)->reset) (a)->reset((a)); } while(0)
#define allocator_debug_str(a, buf, len) (a)->debug ? (a)->debug((a), (buf), (len)) : 0

// heap allocator uses just malloc and free

void allocator_heap_init(allocator *a);
void allocator_heap_reset(allocator *a);
void allocator_heap_clear(allocator *a);
void *allocator_heap_alloc(allocator *a, size_t s ALLOCATOR_PROT);
void *allocator_heap_resize(allocator *a, void *p, size_t s ALLOCATOR_PROT);
void allocator_heap_free(allocator *a, void *p ALLOCATOR_PROT);
char *allocator_heap_str(allocator *a, char *buf, int len);
void allocator_heap_fail(allocator *a ALLOCATOR_PROT);


// debug allocator keeps tracks that you freed every allocation you used

void allocator_dbg_init(allocator *a);
void allocator_dbg_reset(allocator *a);
void allocator_dbg_clear(allocator *a);
void *allocator_dbg_alloc(allocator *a, size_t s ALLOCATOR_PROT);
void *allocator_dbg_resize(allocator *a, void *p, size_t s ALLOCATOR_PROT);
void allocator_dbg_free(allocator *a, void *p ALLOCATOR_PROT);
char *allocator_dbg_str(allocator *a, char *buf, int len);
void allocator_dbg_fail(allocator *a ALLOCATOR_PROT);
size_t allocator_dbg_count(allocator *a);



