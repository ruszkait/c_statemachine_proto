#ifndef LED_STATEMACHINE_H
#define LED_STATEMACHINE_H

#include "message_broker.h"
#include "state_machine.h"

struct led_controller
{
    struct statemachine statemachine;
    // Extra fields come here
};

void led_controller_init(struct led_controller* self);

void led_controller_process_message(service_instance service, message_type msg_type, const int8_t* msg_payload, int msg_payload_size);

#endif

