#pragma once

#include <stdbool.h>
#include "vector.h"

#define objpool(type) struct { vector(struct { type v; bool set; }) objects; vector(unsigned) free_slots; }

#define objpool_free(poolptr) do {			\
		vector_free(&(poolptr)->objects);	\
		vector_free(&(poolptr)->free_slots);	\
	} while(0)


#define objpool_has_free_slots(poolptr) (vector_length(&(poolptr)->free_slots))

#define objpool_slot_is_set(poolptr, index) (vector_get(&(poolptr)->objects, (index))->set)

#define objpool_get_next_free(poolptr)					\
	((vector_get(&(poolptr)->objects, *(vector_get_last(&(poolptr)->free_slots)))->set = 0) \
	 ? NULL								\
	 : (&(vector_get(&(poolptr)->objects, *(vector_pop_last(&(poolptr)->free_slots)))->v)))


#define objpool_add_new(poolptr) \
	(((intptr_t)vector_add(&(poolptr)->objects)) && (vector_get_last(&(poolptr)->objects)->set = 1) \
	 ? &(vector_get_last(&(poolptr)->objects)->v) : NULL)

#define objpool_next_free_slot_from_list(poolptr)			\
	(objpool_has_free_slots(poolptr)				\
	 ? 				\
	 : objpool_add_new(poolptr))

#define objpool_add(poolptr)			\
	(vector_length(&(poolptr)->free_slots)	\
	 ? objpool_get_next_free(poolptr)	\
	 : objpool_add_new(poolptr))

#if 0
#define objpool_get_next_free(poolptr)		\
	(1 + 1 ? NULL : (&(vector_get(&(poolptr)->objects, *(vector_pop_last(&x.free_slots)))->v)))


#define objpool_add_new(poolptr) \
	(((intptr_t)vector_add(&(poolptr)->objects)) \
	 && (vector_get_last(&(poolptr)->objects)->set = 1) \
	 && vector_get_last(&(poolptr)->objects))

#define objpool_next_free_slot_from_list(poolptr)			\
	(objpool_has_free_slots(poolptr)				\
	 ? objpool_get_next_free(poolptr)				\
	 : objpool_add_new(poolptr))

#define objpool_add(poolptr) \
	(vector_length(&(poolptr)->free_slots)				\
	 ? objpool_next_free_slot_from_list(poolptr)			\
	 : objpool_add_new(poolptr))

#endif 

#define objpool_obj_index(poolptr, ptr)					\
	((ptr) - (poolptr)->objects.data)

#define objpool_get(poolptr, index)		\
	(&((poolptr)->objects.data + (index))->v)

#define objpool_remove(poolptr, index) \
	((objpool_slot_is_set(poolptr, index)) && \
	((vector_get(&(poolptr)->objects, (index))->set = 0) || (*(vector_add(&(poolptr)->free_slots)) = (index))))

#define objpool_count(poolptr) \
	((poolptr)->objects.current - (poolptr)->free_slots.current)
