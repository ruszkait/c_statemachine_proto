#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdbool.h>

enum message_types
{
    INIT_SYSTEM,
    TURN_LED,
    START_ADC,
    STOP_ADC,
};

void message_encode_turn_led_payload(void* buffer, int buffer_size, bool turn_on);
void message_decode_turn_led_payload(const void* buffer, int buffer_size, bool* turn_on);

#endif

