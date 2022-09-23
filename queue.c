#include "queue.h"

#include "panic.h"

#include <stdbool.h>
#include <stddef.h>

void queue_init(struct queue* self)
{
    self->begin_index = 0;
    self->end_index = 0;
}

static int advance_index(int index)
{
    int next_index = index + 1;
    if (next_index == MAX_QUEUE_LENGTH)
        next_index = 0;

    return next_index;
}

struct buffer* queue_append(struct queue* self)
{
    const int potential_next_end_index = advance_index(self->end_index);

    panic_if(potential_next_end_index == self->begin_index, "Queue has overflown");

    struct buffer* tail_cell = &self->cells[self->end_index];
    self->end_index = potential_next_end_index;

    return tail_cell;
}

const struct buffer* queue_peek(const struct queue* self)
{
    const bool is_queue_empty = self->begin_index == self->end_index;
    if (is_queue_empty)
        return NULL;

    const struct buffer* head_cell = &self->cells[self->begin_index];

    return head_cell;
}

void queue_pop(struct queue* self)
{
    panic_if(self->begin_index == self->end_index, "The queue cannot be empty when popping");

    self->begin_index = advance_index(self->begin_index);
}
