#include <assert.h>

#include "cog_core.h"
#include "cog_log.h"
#include "cog_state.h"
#include "cog_test.h"

#define STATE_START 0
#define STATE_LEVEL_RUNNING 1
#define STATE_END 2
#define STATE_FINISH 3

#define EVENT_TEST 0
#define EVENT_TEST2 1

int load_firstlevel(void) {
    cog_debugf("loading first level...");
    return STATE_LEVEL_RUNNING;
}

int show_death_screen(void) {
    cog_debugf("showing death screen...");
    return STATE_END;
}

int cleanup_game(void) {
    cog_debugf("cleaning up game...");
    return STATE_FINISH;
}

int infinite_loop(void) {
    cog_debugf("infinite loop state at finish...");
    return STATE_FINISH;
}

cog_state_transition transitions[] = {
    {STATE_START, EVENT_TEST, &load_firstlevel}
    ,
    {STATE_LEVEL_RUNNING, EVENT_TEST, &show_death_screen}
    ,
    {STATE_END, EVENT_TEST2, &cleanup_game}
    ,
    {STATE_FINISH, EVENT_TEST2, &infinite_loop}
    ,
};

int main(void) {
    cog_state_fsm* fsm = cog_state_fsm_alloc();
    cog_state_fsm_add_transitions(fsm, transitions,
                                  (sizeof(transitions) /
                                   sizeof(*transitions)));
    cog_state_fsm_set_state(fsm, STATE_START);
    cog_test("Start state setting", (fsm->currentstate == STATE_START));
    cog_state_fsm_push_event(fsm, EVENT_TEST);
    cog_state_fsm_update(fsm);
    cog_test("Level running transition",
             (fsm->currentstate == STATE_LEVEL_RUNNING));
    cog_state_fsm_push_event(fsm, EVENT_TEST);
    cog_state_fsm_update(fsm);
    cog_test("STATE_END transition", (fsm->currentstate == STATE_END));
    cog_state_fsm_push_event(fsm, EVENT_TEST2);
    cog_state_fsm_update(fsm);
    cog_test("STATE_FINISH transition", (fsm->currentstate == STATE_FINISH));
    cog_state_fsm_push_event(fsm, EVENT_TEST2);
    cog_state_fsm_update(fsm);
    cog_test("STATE_FINISH transition", (fsm->currentstate == STATE_FINISH));
}
