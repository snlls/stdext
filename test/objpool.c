#include "stdext/objpool.h"
#include <stdio.h>
typedef struct {
	int x;
	int y;
} testx;

int test_objpool()
{
	objpool(testx) p = {0};
	printf("filling\n");
	for(int n = 0; n < 16; n++) {
		testx *x = objpool_add(&p);
		x->x = n;
		x->y = n;
	}
	printf("count %d\n", objpool_count(&p));
	for(int n = 0; n < 4; n++) {
		objpool_remove(&p, 8);
	}
	printf("objects %d %d\n", p.objects.allocated, p.objects.current);
	printf("frees %d %d\n", p.free_slots.allocated, p.free_slots.current);
	for(unsigned n = 0; n < p.objects.current; n++) {
		testx *x = &vector_get(&p.objects, n)->v;
		bool set = vector_get(&p.objects, n)->set;
		printf("%d %d %d\n", objpool_get(&p, n)->x, x->x, set);
	}
	for(unsigned n = 0; n < p.free_slots.current; n++) {
		printf("%d\n", *vector_get(&p.free_slots, n));
	}
	objpool_free(&p);
	
	return 0;
}
