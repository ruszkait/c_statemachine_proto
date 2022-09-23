#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H

#include <stdint.h>

#define PAYLOAD_CAPACITY 64

typedef void *service_instance;
typedef int message_type;

struct message_buffer
{
    message_type type;
    service_instance recipient;
    int used_payload;
    int8_t payload[PAYLOAD_CAPACITY];
};

#endif
