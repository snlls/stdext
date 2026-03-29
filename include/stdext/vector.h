#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/** @file
 * @brief Vector implementation.
 * structs current field points to next free slot, so last item in use is current - 1

 @example
 #include "vector.h"

typedef struct {
	int x;
	int y;	
} point;

void test() {
	vector(point) points = {0};
	for(unsigned n = 0; n < 17; n++) {
		point *ptr = vector_add(&points);
		ptr->x = n;
	}
	point *tmp;
	for(unsigned n = 0; n < vector_length(&points); n++) {
		point *tmp = vector_get(&points, n);
	}	
	while((tmp = vector_pop_first(&points))) {
		// do something
	}
	// this also works
	(vector_add(&points))->x = 101;
	vector_remove(&points, 101); // does nothing
	vector_remove(&points, 0); // removes point with value 101
	
	vector_free(&points);
}
 */

#define VECTOR_INIT_SIZE 4

typedef struct {
	unsigned size;
} vector;

#define vector(type)                                                           \
  struct {                                                                     \
    unsigned allocated;                                                        \
    unsigned current;                                                          \
    type *data;                                                                \
  }

/** @brief free allocated data and reset internal state.
 * You can use vector_add after free, it will allocate new data
 */
#define vector_free(vecptr) do {		\
		free((vecptr)->data);		\
		(vecptr)->data = 0;		\
		(vecptr)->allocated = 0;	\
		(vecptr)->current = 0;		\
	} while(0)

/** @brief initial size or sophisticated algorithm for new vector size */
#define vector_new_size(size) ((size) ? (size * 2) : ((4096 / (size)) < VECTOR_INIT_SIZE ? VECTOR_INIT_SIZE : (4096 / (size))))

/** @brief update size values if realloc worked.
    @return original pointer or new pointer.
*/
static void *vector_try_resize(void *data, unsigned *allocated, unsigned type_size)
{
	unsigned newsize = vector_new_size(*allocated);
	void *tmp = realloc(data, newsize * type_size);
	if(tmp) {
		*allocated = newsize;
		return tmp;
	}
	return data;
}

/** @brief return first free slot and increment counter */
#define vector_add_fitting(vecptr) ((vecptr)->data + (vecptr)->current++)

#define vector_has_free_slot(vecptr) ((vecptr)->current < (vecptr)->allocated)


/** @brief if data is not allocated or new item does not fit, allocate new data
 * @return NULL or pointer to added element
 * This would be easier with gcc statement experssion but this implementation supports retarded build systems.
 */
#define vector_add(vecptr)                                                     \
  (vecptr)->data && (vecptr)->current < (vecptr)->allocated                    \
      ? vector_add_fitting(vecptr)                                             \
  : (((vecptr)->data =                                                         \
          vector_try_resize((vecptr)->data, &((vecptr)->allocated),            \
                            sizeof((vecptr)->data[0]))) &&                     \
     vector_has_free_slot(vecptr))                                             \
      ? vector_add_fitting(vecptr)                                             \
      : NULL




#define vector_length(vecptr) (vecptr)->current

/** @brief get pointer to n element or NULL */
#define vector_get(vecptr, n) (((n) < (vecptr)->current && (vecptr)->current) ? (vecptr)->data + (n) : NULL)


/** @brief used internally with pop_first
 * @return retuns true just so it can be used with &&
*/
static bool vector_shift_first_to_last(void *data, unsigned length, unsigned type_size)
{
	char tmp[type_size];
	memcpy(tmp, data, type_size);
	char *ptr = data;
	char *next = ptr + type_size;
	for(unsigned n = 0; n < length - 1; n++) {
		memcpy(ptr, next, type_size);
		ptr += type_size;
		next += type_size;
	}
	// copy tmp element to last slot
	ptr = data;
	ptr += (length - 1) * type_size;
	memcpy(ptr, tmp, type_size);
	return true;
}


/** @brief pop and return pointer to element
 * @return return NULL or pointer to removed element in temporary storage
 * stores the removed element to the new free slot.
 * Shifts all elements.
 * Copies tmp value back to data.
 */
#define vector_pop_first(vecptr)                                               \
  ((vecptr)->current &&                                                        \
           vector_shift_first_to_last((vecptr)->data, (vecptr)->allocated,    \
                                      sizeof((vecptr)->data[0]))               \
       ? ((vecptr)->data + (--((vecptr)->current)))                            \
       : NULL)


/** @brief pop and return pointer to element
 * @return return NULL or pointer to removed element in temporary storage
 */
#define vector_pop_last(vecptr) ((vecptr)->current ? ((vecptr)->data + (--((vecptr)->current))) : NULL)


/** @brief Remove element at index. Shift all elements after index to left. */
#define vector_remove(vecptr, index)                                           \
  do {                                                                         \
    if (!(vecptr)->current || (vecptr)->current <= (index)) {                  \
      break;                                                                   \
    }                                                                          \
    for (unsigned n = (index); n < (vecptr)->current - 1; n++) {               \
      (vecptr)->data[n] = (vecptr)->data[n + 1];                               \
    }                                                                          \
    (vecptr)->current--;                                                       \
  } while (0)

