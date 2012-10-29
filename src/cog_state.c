#include "cog_state.h"

#include "cog_types.h"
#include "cog_core.h"

cog_state_fsm* cog_state_fsm_alloc(void)
{
    cog_state_fsm* obj = COG_STRUCT_MALLOC(cog_state_fsm);
    cog_state_fsm_init(obj);
    return obj;
}

void cog_state_fsm_init(cog_state_fsm* fsm)
{
    cog_list_init(&fsm->transitions, sizeof(cog_state_transition));
    fsm->currentstate = COG_STATE_ERROR;
    cog_list_init(&fsm->events, sizeof(cog_event));
}

void cog_state_fsm_add_transition(cog_state_fsm* fsm, cog_state_transition* transition)
{
    cog_list_append(&fsm->transitions, transition);
}

void cog_state_fsm_add_transitions(cog_state_fsm* fsm, cog_state_transition* transitions, cog_uint size)
{
    cog_int i;
    for(i=0;i<size;i++)
    {
        cog_state_fsm_add_transition(fsm, &(transitions[i]));
    }
}

void cog_state_fsm_update(cog_state_fsm* fsm)
{
    cog_event* event = cog_list_pop(&fsm->events);
    if(event == COG_NULL)
    {
        return;
    }
    COG_LIST_FOREACH(&fsm->transitions)
    {
        cog_state_transition* transition = (cog_state_transition*)curr->data;
        if(transition->state == fsm->currentstate)
        {
            if(transition->event == (*event))
            {
                fsm->currentstate = transition->transition_fn();
                break;
            }
        }
    }
}

void cog_state_fsm_push_event(cog_state_fsm* fsm, cog_event event)
{
    cog_list_append(&fsm->events, &event);
}

void cog_state_fsm_set_state(cog_state_fsm* fsm, cog_state state)
{
    fsm->currentstate = state;
}
