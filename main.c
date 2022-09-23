#include "queue.h"
#include "atomic.h"
#include "messages.h"
#include "led_controller.h"

#include <stdbool.h>

int main()
{
    struct queue message_queue;
    queue_init(&message_queue);

    message_push_init_system(&message_queue);
    message_push_turn_led(&message_queue);
    message_push_turn_led(&message_queue);
    message_push_turn_led(&message_queue);

    struct led_controller led_controller;
    led_controller_init(&led_controller);

    while(true)
    {
        const struct buffer* message = queue_peek(&message_queue);
        const bool no_message = !message;
        if (no_message)
        {
            cpu_sleep();
            continue;
        }

        led_controller_process_signal(&led_controller, message);

        atomic_start();
        queue_pop(&message_queue);
        atomic_end();
    }
}
