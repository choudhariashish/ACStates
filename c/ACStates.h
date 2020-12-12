#include <stdio.h>
#include <stdlib.h>

typedef unsigned short UINT16_t;


// A limit on maximum number of states that a particular
// state can be connected to.
#define MAX_CONNECTED_STATES              10

// Limit on maximum depth of nested/hierarchy of state.
#define MAX_CONNECTED_STATES_STACKSIZE    10

// Default entry event, used by framework.
#define EV_DEFAULT_ENTRY_LIB              0

// Sample EVENTS enum for clients.
// enum EVENTS
// {
//     EV_DEFAULT_ENTRY         = EV_DEFAULT_ENTRY_LIB,
//     EV_USER_1,
//     EV_USER_2,
// }



// Required by framework to know if the state has handled
// an event in its "inprogress" routine.
enum EventState
{
    EVENT_HANDLED,
    EVENT_NOT_HANDLED
};
typedef enum EventState EventState_t;



// An event structure that is passed as an argument to entry,
// inprogress, exit routines.
struct Event
{
    UINT16_t eventId;
    UINT16_t arg0;
    UINT16_t arg1;
};
typedef struct Event Event_t;
typedef EventState_t (*FuncPtr_t)(Event_t);



// State structure.
struct State
{
    FuncPtr_t entry;            // Pointer to entry function.
    FuncPtr_t inprogress;       // Pointer to inprogress function.
    FuncPtr_t exit;             // Ppointer to exit function.
    UINT16_t currSub;           // Used internally by framework.

    struct State *parent;       // Parent of this current state.

    // Array of pointers to connected states.
    struct State *ConnectedStates[MAX_CONNECTED_STATES];
};
typedef struct State State_t;



// Statemachine structure.
struct StateMachine
{
    // Array to hold the active states.
    State_t *StateStack[MAX_CONNECTED_STATES_STACKSIZE];

    // Index of current state in the stack.
    UINT16_t CurrStateIndex;
};
typedef struct StateMachine StateMachine_t;



// API
void prepareState(StateMachine_t *sm,    /* statemachine */
                  State_t *state,        /* state */
                  FuncPtr_t entry,       /* entry function */
                  FuncPtr_t inprogress,  /* inprogress function */
                  FuncPtr_t exit,        /* exit function */
                  State_t *parent);      /* parent of this state */

// Wire the transition from a state to another state on an event.
void addTransition(State_t *fromState, UINT16_t evId, State_t *toState);

// Mark the state as initial state.
void setInitialState(State_t *state);

// Starts the statemachine.
void startMachine( StateMachine_t *sm );

// Triggers an event.
void triggerEvent(StateMachine_t *sm, Event_t ev);
