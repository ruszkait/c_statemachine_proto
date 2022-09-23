#include "message_broker.h"

#include "atomic.h"
#include "panic.h"

#include <stddef.h>
#include <string.h>

void message_broker_init(struct message_broker* self)
{
    self->message_queue_begin_index = 0;
    self->message_queue_end_index = 0;
    memset(&self->registered_services, 0, sizeof(struct service_registration)* MAX_NUM_SERVICES);
}

static int advance_index(int index)
{
    int next_index = index + 1;
    if (next_index == MAX_MESSAGE_QUEUE_LENGTH)
        next_index = 0;

    return next_index;
}

static struct message_buffer* message_broker_queue_append(struct message_broker* self)
{
    const int potential_next_end_index = advance_index(self->message_queue_end_index);

    panic_if(potential_next_end_index == self->message_queue_begin_index, "Queue has overflown");

    struct message_buffer* tail_cell = &self->message_queue[self->message_queue_end_index];
    self->message_queue_end_index = potential_next_end_index;

    return tail_cell;
}

static const struct message_buffer* message_broker_queue_peek(const struct message_broker* self)
{
    const bool is_queue_empty = self->message_queue_begin_index == self->message_queue_end_index;
    if (is_queue_empty)
        return NULL;

    const struct message_buffer* head_cell = &self->message_queue[self->message_queue_begin_index];

    return head_cell;
}

static void message_broker_queue_pop(struct message_broker* self)
{
    panic_if(self->message_queue_begin_index == self->message_queue_end_index, "The message_broker cannot be empty when popping");

    self->message_queue_begin_index = advance_index(self->message_queue_begin_index);
}

static struct service_registration* message_broker_find_service_registration(struct message_broker* self, service_instance instance)
{
    for (int i = 0; i < MAX_NUM_SERVICES; ++i)
    {
        const bool not_the_sought_service = self->registered_services[i].instance != instance;
        if (not_the_sought_service)
            continue;

        return &self->registered_services[i];
    }

    return NULL;
}

void message_broker_register_service(struct message_broker* self, service_instance instance, service_message_processor message_processor)
{
    panic_if(instance == NULL, "message_broker_register_service: invalid instance");
    panic_if(message_processor == NULL, "message_broker_register_service: invalid message message_processor");

    atomic_start();

    struct service_registration* empty_slot = message_broker_find_service_registration(self, NULL);

    const bool no_empty_slot_found = empty_slot == NULL;
    panic_if(no_empty_slot_found, "No free slot found for service");

    empty_slot->instance = instance;
    empty_slot->message_processor = message_processor;

    atomic_end();
}

void message_broker_push_message(struct message_broker* self, service_instance recipient, message_type type, const int8_t* payload, int payload_size)
{
    atomic_start();
    struct message_buffer* allocated_message = message_broker_queue_append(self);
    allocated_message->recipient = recipient;
    allocated_message->type = type;

    panic_if(payload_size > PAYLOAD_CAPACITY, "Payload is too big for the message");

    memcpy(allocated_message->payload, payload, payload_size);
    allocated_message->used_payload = payload_size;

    atomic_end();
}

bool message_broker_dispatch_one(struct message_broker* self)
{
    atomic_start();

    const struct message_buffer* message = message_broker_queue_peek(self);

    const bool no_message = !message;
    if (no_message)
    {
        atomic_end();
        return false;
    }

    for (int i = 0; i < MAX_NUM_SERVICES; ++i)
    {
        const struct service_registration* current_slot = &self->registered_services[i];

        const bool end_service_slot = current_slot->instance == NULL;
        if (end_service_slot)
            break;

        const bool service_is_interested = message->recipient == BROADCAST || current_slot->instance == message->recipient;
        if (!service_is_interested)
            continue;

        atomic_end();

        current_slot->message_processor(current_slot->instance, message->type, message->payload, message->used_payload);

        atomic_start();
    }

    message_broker_queue_pop(self);
    atomic_end();

    return true;
}


