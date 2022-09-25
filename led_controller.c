#include "led_controller.h"
#include "led_controller_signals.h"
#include "message_broker.h"
#include "state_machine.h"

#include <stddef.h>
#include <stdio.h>

// ------------------------------------------------------------------
// Callbacks
// ------------------------------------------------------------------

static void led_controller_configure_port(service_instance service, message_type type, const int8_t* payload, int payload_size)
{
    printf("LED port configured");
}

static void led_controller_turn_led_on(service_instance service, message_type type, const int8_t* payload, int payload_size)
{
    printf("LED on");
}

static void led_controller_turn_led_off(service_instance service, message_type type, const int8_t* payload, int payload_size)
{
    printf("LED off");
}

// ------------------------------------------------------------------
// State machine
// ------------------------------------------------------------------

enum led_controller_states
{
    CREATED,
    LED_ON,
    LED_OFF,
};

static struct transition_rule transition_rules[] = 
{
    {CREATED, NO_SIGNAL, NULL, led_controller_configure_port, LED_OFF},
    {LED_OFF, TURN_LED, NULL, led_controller_turn_led_on, LED_ON},
    {LED_ON, TURN_LED, NULL, led_controller_turn_led_off, LED_OFF},
    {END_GUARD, 0, NULL, NULL, END_GUARD},
};

static struct enter_exit_action_rule enter_exits[] = 
{
    {END_GUARD, NULL, NULL},
};

void led_controller_init(struct led_controller* self)
{
    statemachine_init(&self->statemachine, self, CREATED, transition_rules, enter_exits);
}

void led_controller_process_message(service_instance service, message_type msg_type, const int8_t* msg_payload, int msg_payload_size)
{
    struct led_controller* self = (struct led_controller*)service;
    // Decode message
    enum led_controller_signals signal = msg_type;

    // Dispatch signal to state machine
    statemachine_run_till_idle(&self->statemachine, signal, msg_payload, msg_payload_size);
}

