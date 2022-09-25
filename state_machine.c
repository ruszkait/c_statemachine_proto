#include "state_machine.h"
#include "message_broker.h"

#include <stdbool.h>
#include <stddef.h>

void statemachine_init(struct statemachine* self, void* context, state start_state, struct transition_rule* rules, struct enter_exit_action_rule* enter_exits)
{
    self->current_state = start_state;
    self->transition_rules = rules;
    self->enter_exit_actions = enter_exits;
    self->context = context;

    // If the statemachine needs to be initialized with a triggerless transition
    statemachine_run_till_idle(self, NO_SIGNAL, NULL, 0);
}

static const struct transition_rule* statemachine_find_applying_rule(struct statemachine* self, signal type, const int8_t* payload, int payload_size)
{
    const struct transition_rule* current_rule = self->transition_rules;
    while (true)
    {
        const bool no_more_rules_to_check = current_rule->current_state == END_GUARD;
        if (no_more_rules_to_check)
            break;

        const bool state_matched = current_rule->current_state == self->current_state;
        const bool signal_matched = current_rule->signal == type || current_rule->signal == NO_SIGNAL;

        // It is a design error if multiple rules match, so we dispatch only the first matching rule,
        // and ignore if there are other applying rules.
        const bool rule_did_not_match = !state_matched || !signal_matched;
        if (rule_did_not_match)
        {
            current_rule++;
            continue;
        }

        const bool rule_has_no_guard = current_rule->guard == NULL;
        if (rule_has_no_guard)
            return current_rule;

        const bool guard_passed = current_rule->guard(self->context, type, payload, payload_size);
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

void statemachine_run_till_idle(struct statemachine* self, signal type, const int8_t* payload, int payload_size)
{
    while (true)
    {
        // Find transition rule
        const struct transition_rule* selected_rule = statemachine_find_applying_rule(self, type, payload, payload_size);
        const bool no_applying_rule_found = selected_rule == NULL;
        if (no_applying_rule_found)
            return;

        // Exit current state
        const struct enter_exit_action_rule* old_state_enter_exit_action_rule = statemachine_find_enter_exit_action_rule(self, self->current_state);
        const bool old_state_has_exit_action = old_state_enter_exit_action_rule && old_state_enter_exit_action_rule->exit_action;
        if (old_state_has_exit_action)
            old_state_enter_exit_action_rule->exit_action(self->context);

        // Transition action
        const bool transition_has_action = selected_rule->action != NULL;
        if (transition_has_action)
            selected_rule->action(self->context, type, payload, payload_size);

        // State change
        self->current_state = selected_rule->next_state;

        // Enter new state
        const struct enter_exit_action_rule* new_state_enter_exit_action_rule = statemachine_find_enter_exit_action_rule(self, self->current_state);
        const bool new_state_has_enter_action = new_state_enter_exit_action_rule && new_state_enter_exit_action_rule->enter_action;
        if (new_state_has_enter_action)
            new_state_enter_exit_action_rule->enter_action(self->context);

        // Do further rounds with triggerless transitions
        type = NO_SIGNAL;
    }
}

