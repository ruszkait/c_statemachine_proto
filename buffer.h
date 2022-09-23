#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

#define BUFFER_CAPACITY 64

struct buffer
{
    int used_payload;
    int8_t payload[BUFFER_CAPACITY];
};

#endif
