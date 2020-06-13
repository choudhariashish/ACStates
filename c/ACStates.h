#include <stdio.h>
#include <stdlib.h>

typedef unsigned short UINT16_t;

#define MAX_CONNECTED_STATES              10
#define MAX_CONNECTED_STATES_STACKSIZE    10
#define EV_DEFAULT_ENTRY_LIB              0

// Sample EVENTS enum for clients.
// enum EVENTS
// {
//     EV_DEFAULT_ENTRY         = EV_DEFAULT_ENTRY_LIB,
//     EV_USER_1,
//     EV_USER_2,
// }


struct Event
{
    UINT16_t eventId;
    UINT16_t arg0;
    UINT16_t arg1;
};

typedef struct Event Event_t;
typedef void (*FuncPtr_t)(Event_t);


enum StateSubroutine
{
    ENTRY,
    INPROGRESS,
    EXIT
};

struct State
{
    FuncPtr_t entry;
    FuncPtr_t inprogress;
    FuncPtr_t exit;
    UINT16_t currSub;

    struct State *parent;
    struct State *ConnectedStates[MAX_CONNECTED_STATES];
};
typedef struct State State_t;


struct StateMachine
{
    State_t *StateStack[MAX_CONNECTED_STATES_STACKSIZE];
    UINT16_t CurrStateIndex;
};
typedef struct StateMachine StateMachine_t;



// API
void prepareState(StateMachine_t *sm,
                  State_t *state,
                  FuncPtr_t entry,
                  FuncPtr_t inprogress,
                  FuncPtr_t exit,
                  State_t *parent);

void addTransition(State_t *fromState, UINT16_t evId, State_t *toState);
void setInitialState(State_t *state);
void startMachine( StateMachine_t *sm );
void triggerEvent(StateMachine_t *sm, Event_t ev);
