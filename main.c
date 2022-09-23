#include "message_broker.h"
#include "atomic.h"
#include "messages.h"
#include "led_controller.h"

#include <stdbool.h>

int main()
{
    struct message_broker message_broker;
    message_broker_init(&message_broker);

    message_broker_push_message(&message_broker, BROADCAST, INIT_SYSTEM, NULL, 0);

    struct led_controller led_controller;
    led_controller_init(&led_controller);

    message_broker_register_service(&message_broker, &led_controller, led_controller_process_signal);

    message_broker_push_message(&message_broker, &led_controller, TURN_LED, NULL, 0);
    message_broker_push_message(&message_broker, &led_controller, TURN_LED, NULL, 0);
    message_broker_push_message(&message_broker, &led_controller, TURN_LED, NULL, 0);

    while(true)
    {
        const bool messages_pending = message_broker_dispatch_one(&message_broker);
        if (!messages_pending)
            cpu_sleep();
    }
}
