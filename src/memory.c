// for strdup
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdext/memory.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

void (*cc_memory_fault_handler)() = cc_memory_fault_handler_default;

typedef struct allocator_dbg_block allocator_dbg_block;
struct allocator_dbg_block {
	allocator_dbg_block *prev;
	allocator_dbg_block *next;
	void *p;
	size_t size;
	char *file;
	char *func;
	int line;
};

void allocator_dbg_fail(allocator *a ALLOCATOR_PROT)
{
#if defined(ALLOCATOR_ADD_LINES)
	const int l = line;
	const char *fl = file;
	const char *fn = func;
#else
	const int l = 0;
	const char *fl = 0;
	const char *fn = 0;

#endif

	fprintf(stderr, "dbg allocator failed %s%d (%s) , aborting\n", fl, l, fn);
	fflush(stderr);
	abort();
}

void allocator_dbg_init(allocator *a)
{
	a->alloc = allocator_dbg_alloc;
	a->resize = allocator_dbg_resize;
	a->free = allocator_dbg_free;
	a->fail = allocator_dbg_fail;
	a->reset = allocator_dbg_reset;
	a->debug = allocator_dbg_str;
	a->data = 0;
	mtx_init(&a->lock, mtx_plain);
}

void allocator_dbg_reset(allocator *a)
{
	mtx_lock(&a->lock);
	allocator_dbg_block *b = a->data;
	unsigned count = 0;
	while(b) {
		count++;
		b = b->next;
	}
	if(!count) {
		mtx_unlock(&a->lock);
		mtx_destroy(&a->lock);
		return;
	}
	b = a->data;
	fprintf(stderr, "Destroying dbg allocator\t still allocated %u\n", count);
	while(b) {
		fprintf(stderr, "\t%s:%d %s size %lu\n", b->file, b->line, b->func, b->size);
		if(b->file)
			free(b->file);
		if(b->func)
			free(b->func);
		void *next = b->next;
		free(b);
		b = next;
	}
	a->data = 0;
	mtx_unlock(&a->lock);
	mtx_destroy(&a->lock);
}

void *allocator_dbg_alloc(allocator *a, size_t size ALLOCATOR_PROT)
{
#if defined(ALLOCATOR_ADD_LINES)
	const int l = line;
	const char *fl = file;
	const char *fn = func;
#else
	const int l = 0;
	const char *fl = 0;
	const char *fn = 0;

#endif
	if(!size) {
		return 0;
	}
	mtx_lock(&a->lock);
	allocator_dbg_block *b = calloc(1, sizeof(*b) + size);
	if(!b) {
		mtx_unlock(&a->lock);
		if(a->fail) {
			a->fail(a ALLOCATOR_ARGS_CALL);
		}
		else {
			perror("allocation failed");
			cc_memory_fault_handler();
		}
		return 0;
	}
	if(fl)
		b->file = strdup(fl);
	if(fn)
		b->func = strdup(fn);

	b->line = l;
	b->size = size;
	b->p = (b + 1);
	if(!a->data) {
		a->data = b;
		b->next = 0;
		b->prev = 0;
	}
	else {
		allocator_dbg_block *last = a->data;
		while(last->next)
			last = last->next;
		last->next = b;
		b->prev = last;
		b->next = 0;
	}
	mtx_unlock(&a->lock);
	return (b + 1);
}
void *allocator_dbg_resize(allocator *a, void *p, size_t size ALLOCATOR_PROT)
{
	if(!size) {
		allocator_dbg_free(a, p ALLOCATOR_ARGS_CALL);
		return 0;
	}
	if(!p) {
		return allocator_dbg_alloc(a, size ALLOCATOR_ARGS_CALL);
	}
	allocator_dbg_block *b = ((allocator_dbg_block *)p) - 1;
	void *newp = allocator_dbg_alloc(a, size ALLOCATOR_ARGS_CALL);

	memcpy(newp, b->p, size < b->size ? size : b->size);
	allocator_dbg_free(a, p ALLOCATOR_ARGS_CALL);
	return newp;
}
void allocator_dbg_free(allocator *a, void *p ALLOCATOR_PROT)
{
	if(!p)
		return;

	mtx_lock(&a->lock);
	allocator_dbg_block *b = ((allocator_dbg_block *)p) - 1;
	if(b->file)
		free(b->file);
	if(b->func)
		free(b->func);
	allocator_dbg_block *prev = b->prev;
	if(prev)
		prev->next = b->next;
	else
		a->data = b->next;
	if(b->next)
		b->next->prev = prev;
	mtx_unlock(&a->lock);
	free(b);
}
char *allocator_dbg_str(allocator *a, char *buf, int len)
{
	mtx_lock(&a->lock);
	allocator_dbg_block *b = a->data;
	unsigned count = 0;
	while(b) {
		count++;
		b = b->next;
	}
	char *c = buf;
	int tmp = snprintf(c, len, "dbg allocator %p count %u\n", a, count);
	c += tmp;
	len -= tmp;
	b = a->data;
	while(b && len > 0) {
		printf("%p %d %d\n", b, tmp, len);
		tmp = snprintf(c, len, "\n\t%s:%d %s size %lu", b->file, b->line, b->func, b->size);
		c += tmp;
		len -= tmp;
		b = b->next;
	}
	mtx_unlock(&a->lock);
	return buf;
}

size_t allocator_dbg_count(allocator *a)
{
	if(!a)
		return 0;

	mtx_lock(&a->lock);
	allocator_dbg_block *b = a->data;
	size_t ret = 0;
	while(b) {
		ret++;
		b = b->next;
	}
	mtx_unlock(&a->lock);
	return ret;
}

void allocator_heap_fail(allocator *a ALLOCATOR_PROT)
{
	perror("allocator failed, aborting");
	abort();
}

void allocator_heap_init(allocator *a)
{
	memset(a, 0, sizeof(*a));
	a->free = allocator_heap_free;
	a->alloc = allocator_heap_alloc;
	a->resize = allocator_heap_resize;
	a->fail = allocator_heap_fail;
	a->reset = allocator_heap_reset;
	a->debug = allocator_heap_str;
}

void allocator_heap_reset(allocator *a) { allocator_heap_clear(a); }

void allocator_heap_clear(allocator *a) { (void)a; }

void *allocator_heap_alloc(allocator *a, size_t s ALLOCATOR_PROT)
{
	(void)a;
	void *p = calloc(1, s);
	if(!p) {
		if(a->fail) {
			a->fail(a ALLOCATOR_ARGS_CALL);
			return 0;
		}
		else {
			perror("allocation failed");
			cc_memory_fault_handler();
			return 0;
		}
	}
	return p;
}

void *allocator_heap_resize(allocator *a, void *p, size_t s ALLOCATOR_PROT)
{
	(void)a;
	return realloc(p, s);
}

void allocator_heap_free(allocator *a, void *p ALLOCATOR_PROT)
{
	(void)a;
	free(p);
}

char *allocator_heap_str(allocator *a, char *buf, int len)
{
	if(len)
		buf[0] = 0;

	return buf;
}
