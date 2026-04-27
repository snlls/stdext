#pragma once
#include <stdbool.h>
#include <string.h>

#define kvfield(key_type, value_type) \
	struct { bool set; key_type key; value_type value; }

#define kvmap(key_type, value_type, mcount) \
	struct { kvfield(key_type, value_type) map[(mcount)]; unsigned count;  }

#define kvmap_find(mapptr, key_ptr, entry_result_ptr)\
	for(unsigned n = 0; n < sizeof((mapptr)->map / (mapptr)->map[0]); n++) { \
		if(!(mapptr)->map[n].set)				\
			continue;						\
		if(!memcmp(&(mapptr)->map[n].key, key_ptr, sizeof((mapptr)->map[n].key))) { \
			memcpy(entry_result_ptr, &(mapptr)->map[n].value); \
			break;						\
		}							\
	}


#define kvmap_add(mapptr, key_ptr, value_ptr)\
	for(unsigned n = 0; n < sizeof((mapptr)->map) / sizeof((mapptr)->map[0]); n++) { \
		if((mapptr)->map[n].set)				\
			continue;					\
		memcpy(&(mapptr)->map[n].key, key_ptr, sizeof((mapptr)->map[n].key)); \
		memcpy(&(mapptr)->map[n].value, value_ptr, sizeof((mapptr)->map[n].value)); \
		(mapptr)->map[n].set = 1;				\
		(mapptr)->count++;					\
		break;							\
	}


#define kvmap_rem(mapptr, key_ptr)\
	for(unsigned n = 0; n < sizeof((mapptr)->map / (mapptr)->map[0]); n++) { \
		if(!(mapptr)->map[n].set)				\
			continue;					\
		if(!memcmp(&(mapptr)->map[n].key, key_ptr, sizeof((mapptr)->map[n].key))) { \
			memset((mapptr)->map + n, 0, sizeof((mapptr)->map[n]));	\
			(mapptr)->count--;				\
			break;						\
		}							\
	}
