#ifndef QUEUE_H
#define QUEUE_H

#include "buffer.h"

#define MAX_QUEUE_LENGTH 64
struct queue
{
    struct buffer cells[MAX_QUEUE_LENGTH];
    int begin_index;
    int end_index;
};

void queue_init(struct queue* self);

// Must be synched with pop
struct buffer* queue_append(struct queue* self);

// Need not to be atomic
const struct buffer* queue_peek(const struct queue* self);

// Must be synched with append
void queue_pop(struct queue* self);

#endif

