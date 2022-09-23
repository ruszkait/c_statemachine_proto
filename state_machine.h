#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "buffer.h"
#include "queue.h"

#include <stdbool.h>

typedef int state;

#define END_GUARD -1

struct transition_rule
{
    state current_state;
    int signal;
    bool(*guard)(void* context, const struct buffer* message);
    void (*action)(void* context, const struct buffer* message);
    state next_state;
};

struct enter_exit_action_rule
{
    state state;
    void (*enter)(void* context);
    void (*exit)(void* context);
};

struct statemachine
{
    int current_state;
    const struct transition_rule* transition_rules;
    const struct enter_exit_action_rule* enter_exit_actions;
    void* context;
};

void statemachine_init(struct statemachine* self, void* context, state start_state, struct transition_rule* rules, struct enter_exit_action_rule* enter_exits);

void statemachine_process_signal(struct statemachine* self, const struct buffer* message);

#endif

