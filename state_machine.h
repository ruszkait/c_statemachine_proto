#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdbool.h>
#include <stdint.h>

typedef int state;
typedef int signal;

#define END_GUARD -1
#define NO_SIGNAL -2

typedef void (*action)(void* context, signal type, const int8_t* payload, int payload_size);
typedef bool (*guard)(void* context, signal type, const int8_t* payload, int payload_size);

struct transition_rule
{
    state current_state;
    int signal;
    guard guard;
    action action;
    state next_state;
};

typedef void (*enter_action)(void* context);
typedef void (*exit_action)(void* context);

struct enter_exit_action_rule
{
    state state;
    enter_action enter_action;
    exit_action exit_action;
};

struct statemachine
{
    int current_state;
    const struct transition_rule* transition_rules;
    const struct enter_exit_action_rule* enter_exit_actions;
    void* context;
};

void statemachine_init(struct statemachine* self, void* context, state start_state, struct transition_rule* rules, struct enter_exit_action_rule* enter_exits);

void statemachine_run_till_idle(struct statemachine* self, signal type, const int8_t* payload, int payload_size);

#endif

