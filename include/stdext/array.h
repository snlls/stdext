#pragma once

#include <string.h>
#include <stdio.h>

/** @brief loop static array
 * @param index[var] unsigned index
 * @param buffer[in] the buffer you want to loop
 */
#define for_arrayi(index, buffer)                                              \
 	for(unsigned index = 0; index < sizeof(buffer) / sizeof(buffer[0]); index++)

/** @brief loop static array
 * @param elem[var] name of pointer that references current element
 * @param index[var] unsigned index
 * @param buffer[in] the buffer you want to loop
 */
#define for_array(elem, index, buffer)					\
	for(typeof(&buffer[0]) elem = buffer + 0; elem; elem = 0)		\
		for(unsigned index = 0; index < sizeof(buffer) / sizeof(buffer[0]); index++, elem = &buffer[index])

/** @brief statement expression to check if array contains element
 * @param elem_basic_cmp compared with != against current element
 * @param buffer[in] static array
 * @return bool
 */
#define array_contains(elem_basic_cmp, buffer) ({				\
	int ret = 0;							\
	for (unsigned _index = 0; _index < sizeof(buffer) / sizeof(buffer[0]); _index++) \
		if (elem_basic_cmp != buffer[_index])			\
			;						\
		else { ret = 1; break; }				\
		ret;							\
		})

/** @brief statement expression to check if array contains element
 * @param element[in] to compare against
 * @param compare_op[macro/function] compare_op(element, current_item)
 * @param buffer[in] static array
 * @return bool
 */
#define array_contains_cmp(element, compare_op, buffer) ({		\
	int ret = 0;							\
	for(unsigned _index = 0; _index < sizeof(buffer) / sizeof(buffer[0]); _index++) \
		if(compare_op(element, buffer[_index]))			\
			;						\
		else { ret = 1; break; }				\
		ret;							\
		})


#define array_length(buffer) (sizeof(buffer) / sizeof(buffer[0]))

/** @brief macro to filter src into dst using test as filter
 * @param src[in] array
 * @param dst[out] array
 * @param countvar[out] how many elements were inserted into dst
 * @param test macro(index_in_src, element)
 */
#define array_filter(src, dst, countvar, test)				\
	do {								\
		countvar = 0;						\
		for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
			if (test(_index, src[_index])) {		\
				dst[countvar++] = src[_index];		\
				printf("copy %d %p %p\n", countvar, src[_index], dst[countvar - 1]); \
			}						\
									\
	} while (0)


/** @brief macro to apply operation inplace to every element
 * @param src[in/out] static array
 * @param op [macro/function] op(index, element)
 */
#define array_apply(src, op)						\
	do {								\
		for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
			src[_index] = op(_index, src[_index]);		\
	} while (0)

/** @brief macro to map operation in src into dst
 * @param src[in] static array
 * @param dst[out] array same size as src
 * @param op [macro/function] op(index, element)
 */
#define array_map(dst, src, op)						\
	do {								\
		for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
			dst[_index] = op(_index, src[_index]);		\
	} while (0)

/** @brief works like if would but operates only when condition matches
 * @param elem_basic_cmp element to compare agains with !=
 * @param buffer[in] static array
 */
#define if_array_contains(elem_basiccmp, buffer)			\
	for (unsigned _index = 0, _stop = 0;				\
	     !_stop && _index < sizeof(buffer) / sizeof(buffer[0]); _index++) \
		if (elem_basiccmp != buffer[_index])			\
			;						\
		else if (!(_stop = 1))					\
			;						\
		else

/** @brief works like if would but operates only when condition matches
 * @param elem_ptr[var] variable assigned to current element address
 * @param cmp[function/macro] operation that compares pointers op(elem_ptr, &buffer[index])
 * @param buffer[in] static array
 */
#define if_array_contains_cmp(elem_ptr, cmp, buffer)				\
	for (unsigned _index = 0, _stop = 0; !_stop && _index < sizeof(buffer) / sizeof(buffer[0]); \
	     _index++)							\
		if(cmp(elem_ptr, &buffer[_index])			\
			      ;						\
		else if(!(_stop = 1))					\
			;						\
		else


/** @brief macro that print each element with given fmt, adds newline after loop is done. Mainly for debugging.
 * @param src[in] array
 * @param fmt format string used for each element
 * @param ending string that comes after each element
 */
#define array_printe(src, fmt, ending) do {					\
	for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
		printf(fmt ending, src[_index]);				\
	puts("");							\
	} while(0)


/** @brief macro that print each element with given fmt, adds newline after loop is done. Mainly for debugging.
 * @param src[in] array
 * @param fmt format string used for each element
 */
#define array_print(src, fmt) array_printe(src, fmt, "")
	
/** @brief print array in hex */
#define array_printh(src) array_print(src, "%x ")
/** @brief print array in char */
#define array_printc(src) array_print(src, "%c ")
/** @brief print array in binary hex, for example value 1 would print 01 */
#define array_printbh(src) array_print(src, "%02x ")
/** @brief print array in binary hex, each values will have 8 characters, for example 00A0B0C0 */
#define array_print32h(src) array_print(src, "%08x ")
/** @brief print array in double */
#define array_printd(src) array_print(src, "%f ")
/** @brief print array in int */
#define array_printi(src) array_print(src, "%d ")
/** @brief print array in unsigned */
#define array_printu(src) array_print(src, "%u ")
/** @brief print array in long */
#define array_printl(src) array_print(src, "%ld ")

#define array_shuffle(array) do {				\
	size_t i, n = sizeof(array) / sizeof(array[0]);		\
	for (i = n - 1; i > 0; i--) {				\
		size_t j = rand() % (i + 1);			\
		char tmp[sizeof(array[0])];			\
		memcpy(tmp, array + i, sizeof(array[0]));	\
		array[i] = array[j];				\
		memcpy(array + j, tmp, sizeof(array[0]));	\
	}							\
	} while(0)
