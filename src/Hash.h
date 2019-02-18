#ifndef _HASH_H
#define _HASH_H

#include <stddef.h>

typedef unsigned int hash_t;

hash_t hash (unsigned int crc, const void *buf, size_t size);

#endif
