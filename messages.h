#ifndef MESSAGES_H
#define MESSAGES_H

#include "buffer.h"
#include "queue.h"

#include <stdbool.h>

enum message_types
{
    INIT_SYSTEM,
    TURN_LED,
    START_ADC,
    STOP_ADC,
};

void message_push_init_system(struct queue* queue);
void message_push_turn_led(struct queue* queue);


enum message_types message_decode_type(const struct buffer* buffer);
void message_decode_turn_led(struct buffer* buffer, bool* turn_on);
void message_decode_start_adc(struct buffer* buffer, int* sampling_rate);

#endif

