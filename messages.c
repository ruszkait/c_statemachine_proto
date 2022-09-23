#include "messages.h"

#include "atomic.h"
#include "panic.h"

struct message_header
{
    enum message_types message_type;
};

static void message_encode_parameterless(struct buffer* buffer, enum message_types message_type)
{
    panic_if(sizeof(struct message_header) > BUFFER_CAPACITY, "Parameterless message did not fit into the queue cell");

    struct message_header* message = (struct message_header*)buffer->payload;
    message->message_type = message_type;
    
    buffer->used_payload = sizeof(struct message_header);
}

void message_push_init_system(struct queue* queue)
{
    atomic_start();
    message_encode_parameterless(queue_append(queue), INIT_SYSTEM);
    atomic_end();
}

void message_push_turn_led(struct queue* queue)
{
    atomic_start();
    message_encode_parameterless(queue_append(queue), TURN_LED);
    atomic_end();
}


enum message_types message_decode_type(const struct buffer* buffer)
{
    const struct message_header* message = (const struct message_header*)buffer->payload;
    return message->message_type;
}