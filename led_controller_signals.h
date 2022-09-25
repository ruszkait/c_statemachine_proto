#ifndef LED_CONTROLLER_SIGNALS_H
#define LED_CONTROLLER_SIGNALS_H

enum led_controller_signals
{
    TURN_LED,
    START_ADC,
    STOP_ADC,
};

void message_encode_turn_led_payload(void* buffer, int buffer_size, bool turn_on);
void message_decode_turn_led_payload(const void* buffer, int buffer_size, bool* turn_on);

#endif

