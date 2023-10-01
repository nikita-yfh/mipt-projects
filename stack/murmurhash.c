#include "murmurhash.h"

#include <assert.h>

unsigned int murmurHash2 (const void* key, unsigned int len) {
	assert(key);

	const unsigned int m = 0x5bd1e995;
	const unsigned int seed = 0;
	const unsigned int r = 24;

	unsigned int h = seed ^ len;

	const unsigned char * data = (const unsigned char *)key;
	unsigned int k = 0;

	while (len >= 4) {
		k  = (unsigned int) data[0];
		k |= (unsigned int) data[1] << 8;
		k |= (unsigned int) data[2] << 16;
		k |= (unsigned int) data[3] << 24;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch (len) {
		case 3:
			h ^= (unsigned int) data[2] << 16;
			[[fallthrough]];
		case 2:
			h ^= (unsigned int) data[1] << 8;
			[[fallthrough]];
		case 1:
			h ^= data[0];
			h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
