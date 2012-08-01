#include <assert.h>

#include "cog_state.h"

#define STATE_LEVEL_RUNNING 0
#define STATE_END 1

#define EVENT_TEST 0

static cog_int load_firstlevel(void)
{
    printf("loading first level...");
    return STATE_LEVEL_RUNNING;
}

static cog_int show_death_screen(void)
{
    printf("showing death screen...");
    return STATE_END;
}

static cog_int cleanup_game(void)
{
    printf("cleaning up game...");
    return COG_STATE_FINISH;
}

cog_state_transition transitions[] = {
    { COG_STATE_START, EV_TESTEVENT, &load_firstlevel },
    { STATE_LEVEL_RUNNING, EV_TESTEVENT, &show_death_screen },
    { STATE_END, EV_TESTEVENT, &cleanup_game },
};

int main(void)
{
    cog_state_fsm fsm* cog_state_fsm_alloc();
    cog_state_fsm_add_transitions(fsm, transitions, (sizeof(transitions)/sizeof(*transitions)));

    assert(fsm->currentstate==COG_STATE_START && "Start state not properly set");
    cog_state_fsm_push_event(fsm, TEST_EVENT);
    cog_state_fsm_update(fsm);
    assert(fsm->currentstate==STATE_LEVEL_RUNNING && "Level Running transition not working");
    cog_state_fsm_push_event(fsm, TEST_EVENT);
    cog_state_fsm_update(fsm);
    assert(fsm->currentstate==STATE_END && "STATE_END transition not working.");
    cog_state_fsm_push_event(fsm, TEST_EVENT);
    cog_state_fsm_update(fsm);
    assert(fsm->currentstate==COG_STATE_FINISH && "COG_STATE_FINISH transition not working.");
}

