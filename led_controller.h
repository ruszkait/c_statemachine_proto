#ifndef LED_STATEMACHINE_H
#define LED_STATEMACHINE_H

#include "queue.h"
#include "state_machine.h"

struct led_controller
{
    struct statemachine statemachine;
    // Extra fields come here
};

void led_controller_init(struct led_controller* self);

void led_controller_process_signal(struct led_controller* self, const struct buffer* message);

#endif

