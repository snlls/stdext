#include <ccpsx/memory.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#define elog(fmt, ...)                                                                                                 \
	do {                                                                                                           \
		printf("%s:%d " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);                                          \
	} while(0)

static int memory_test_failed = 0;
void memory_test_failed_fn() { memory_test_failed = 1; }

int memory_test()
{
	int errors = 0;
	allocator a;
	cc_memory_fault_handler = memory_test_failed_fn;
	void *pfail = cc_alloc(LONG_MAX);
	if(pfail || !memory_test_failed) {
		elog("memory fail test failed p=%p flag=%d", pfail, memory_test_failed);
	}

	allocator_dbg_init(&a);
	int *p = allocator_alloc(&a, sizeof(int));
	if(!p) {
		elog("dbg allocation failed");
		errors++;
	}
	p = allocator_resize(&a, p, 2 * sizeof(int));
	if(!p) {
		elog("dbg allocation resize");
		errors++;
	}
	allocator_free(&a, p);
	allocator_reset(&a);

	allocator_heap_init(&a);
	p = allocator_alloc(&a, sizeof(int));
	if(!p) {
		elog("heap allocation failed");
		errors++;
	}

	p = allocator_resize(&a, p, 2 * sizeof(int));
	if(!p) {
		elog("heap allocation resize");
		errors++;
	}
	allocator_free(&a, p);
	allocator_reset(&a);

	allocator_dbg_init(&a);
	p = allocator_alloc(&a, sizeof(int));
	p = allocator_alloc(&a, sizeof(int));
	size_t size = allocator_dbg_count(&a);
	if(size != 2) {
		elog("allocation count was wrong");
		errors++;
	}
	allocator_reset(&a);
	return errors;
}
