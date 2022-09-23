#ifndef MESSAGE_BROKER_H
#define MESSAGE_BROKER_H

#include <stdbool.h>

#include "message_buffer.h"

// Message broker config
#define MAX_NUM_SERVICES 20
#define MAX_MESSAGE_QUEUE_LENGTH 64


typedef void (*service_message_processor)(service_instance service, message_type type, const int8_t* payload, int payload_size);
#define BROADCAST (void*)-1

struct service_registration
{
    service_instance instance;
    service_message_processor message_processor;
};

struct message_broker
{
    struct message_buffer message_queue[MAX_MESSAGE_QUEUE_LENGTH];
    int message_queue_begin_index;
    int message_queue_end_index;
    struct service_registration registered_services[MAX_NUM_SERVICES];
};

void message_broker_init(struct message_broker* self);

bool message_broker_dispatch_one(struct message_broker* self);

void message_broker_push_message(struct message_broker* self, service_instance recipient, message_type type, const int8_t* payload, int payload_size);

void message_broker_register_service(struct message_broker* self, service_instance instance, service_message_processor message_processor);

#endif

