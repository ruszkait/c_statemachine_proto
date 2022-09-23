#include "state_machine.h"
#include "messages.h"
#include "queue.h"

#include <stdbool.h>
#include <stddef.h>

void statemachine_init(struct statemachine* self, void* context, state start_state, struct transition_rule* rules, struct enter_exit_action_rule* enter_exits)
{
    self->current_state = start_state;
    self->transition_rules = rules;
    self->enter_exit_actions = enter_exits;
    self->context = context;
}

static const struct transition_rule* statemachine_find_applying_rule(struct statemachine* self, const struct buffer* message)
{
    const enum message_types message_t = message_decode_type(message);

    const struct transition_rule* current_rule = self->transition_rules;
    while (true)
    {
        const bool no_more_rules_to_check = current_rule->current_state == END_GUARD;
        if (no_more_rules_to_check)
            break;

        const bool state_matched = current_rule->current_state == self->current_state;
        const bool signal_matched = current_rule->signal == message_t;

        const bool rule_did_not_match = !state_matched || !signal_matched;
        if (rule_did_not_match)
        {
            current_rule++;
            continue;
        }

        const bool rule_has_no_guard = current_rule->guard == NULL;
        if (rule_has_no_guard)
            return current_rule;

        const bool guard_passed = current_rule->guard(self->context, message);
        if (guard_passed)
            return current_rule;
    }

    return NULL;
}

static const struct enter_exit_action_rule* statemachine_find_enter_exit_action_rule(struct statemachine* self, state state)
{
    const struct enter_exit_action_rule* current_enter_exit = self->enter_exit_actions;
    while (true)
    {
        const bool no_more_enter_exit_to_check = current_enter_exit->state == END_GUARD;
        if (no_more_enter_exit_to_check)
            break;

        const bool state_matched = current_enter_exit->state == state;

        if (state_matched)
            return current_enter_exit;
    }

    return NULL;
}

void statemachine_process_signal(struct statemachine* self, const struct buffer* message)
{
    // Find transition rule
    const struct transition_rule* selected_rule = statemachine_find_applying_rule(self, message);
    const bool no_applying_rule_found = selected_rule == NULL;
    if (no_applying_rule_found)
        return;

    // Exit current state
    const struct enter_exit_action_rule* old_state_enter_exit_action_rule = statemachine_find_enter_exit_action_rule(self, self->current_state);
    const bool old_state_has_exit_action = old_state_enter_exit_action_rule && old_state_enter_exit_action_rule->exit;
    if (old_state_has_exit_action)
        old_state_enter_exit_action_rule->exit(self->context);

    // Transition action
    const bool transition_has_action = selected_rule->action != NULL;
    if (transition_has_action)
        selected_rule->action(self->context, message);

    // State change
    self->current_state = selected_rule->next_state;

    // Enter new state
    const struct enter_exit_action_rule* new_state_enter_exit_action_rule = statemachine_find_enter_exit_action_rule(self, self->current_state);
    const bool new_state_has_enter_action = new_state_enter_exit_action_rule && new_state_enter_exit_action_rule->enter;
    if (new_state_has_enter_action)
        new_state_enter_exit_action_rule->enter(self->context);
}

