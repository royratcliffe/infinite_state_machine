#include "infinite_state_machine.h"

#include <stdbool.h>
#include <assert.h>

static void engine_cycle(void);
static void igniting_cycle(void);
static void cranking_cycle(void);

static void starting_enter(struct infinite_state *state, struct infinite_state_machine *machine);
static void igniting_enter(struct infinite_state *state, struct infinite_state_machine *machine);
static void cranking_enter(struct infinite_state *state, struct infinite_state_machine *machine);

struct engine
{
    struct infinite_state state;
    void (*cycle)(void);
};

struct starting
{
    struct engine engine;
    int cycling;
};

/*
 * Initialise the engine state topology.
 */
static struct engine stopped = {.cycle = engine_cycle};
static struct engine starting = {.state.enter = starting_enter, .cycle = engine_cycle};
static struct starting igniting = {.engine = {.state.super = &starting.state, .state.enter = igniting_enter, .cycle = igniting_cycle}};
static struct starting cranking = {.engine = {.state.super = &starting.state, .state.enter = cranking_enter, .cycle = cranking_cycle}};
static struct engine running = {.cycle = engine_cycle};

static struct infinite_state_machine engine;

static void go(struct engine *to)
{
    infinite_state_machine_goto(&engine, &to->state);
}

/*
 * Check if the engine is in a specific state or super-state.
 */
static bool in(struct engine *in)
{
    return infinite_state_machine_in(&engine, &in->state) == 1;
}

/*
 * Start the engine. This is an external event.
 * Transition from the stopped state to the starting state.
 */
void start(void)
{
    if (in(&stopped))
    {
        go(&starting);
    }
}

/*
 * Stop the engine, an external event.
 * Transition from the any state to the stopped state.
 */
void stop(void)
{
    go(&stopped);
}

/*
 * Cycle the engine's top state.
 * This takes advantage of the fact that the engine's states are all of the same
 * type. It assumes that the top state is always an engine structure and that
 * the structure's first member is the infinite_state base structure. It also
 * assumes that the cycle function is implemented for each engine state.
 */
static void cycle(void)
{
    ((struct engine *)infinite_state_machine_top(&engine))->cycle();
}

/*
 * Test the infinite state machine.
 */
int test_engine()
{
    /*
     * Strictly speaking, the machine does not require initialisation.
     * It lives in "blank static storage." The C run-time automatically initialises
     * all static and global variables to zero. In other words, C has already initialised
     * the machine for us.
     */
    infinite_state_machine_init(&engine);

    /*
     * Set up the engine by applying the initial transition.
     */
    go(&stopped);

    assert(in(&stopped));
    start();
    assert(in(&starting));
    assert(in(&igniting.engine));
    cycle();
    assert(in(&starting));
    assert(in(&cranking.engine));
    cycle();
    assert(in(&starting));
    assert(in(&cranking.engine));
    cycle();
    assert(in(&running));
    stop();
    assert(in(&stopped));

    return 0;
}

static void engine_cycle(void) {}

static void igniting_cycle(void)
{
    if (--igniting.cycling == 0)
    {
        go(&cranking.engine);
    }
}

static void cranking_cycle(void)
{
    if (--cranking.cycling == 0)
    {
        go(&running);
    }
}

static void starting_enter(struct infinite_state *state, struct infinite_state_machine *machine)
{
    go(&igniting.engine);
}

static void igniting_enter(struct infinite_state *state, struct infinite_state_machine *machine)
{
    igniting.cycling = 1;
}

static void cranking_enter(struct infinite_state *state, struct infinite_state_machine *machine)
{
    cranking.cycling = 2;
}
