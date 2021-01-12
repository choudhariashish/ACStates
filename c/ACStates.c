#include "ACStates.h"

void prepareState(StateMachine_t *sm,
                  State_t *state,
                  FuncPtr_t entry,
                  FuncPtr_t inprogress,
                  FuncPtr_t exit,
                  State_t *parent)
{
    state->entry = entry;
    state->inprogress = inprogress;
    state->exit = exit;
    state->parent = parent;

    // Reset event array to NULL states.
    UINT16_t ev = 0;
    for (ev = 0; ev < MAX_CONNECTED_STATES; ev++)
    {
        state->ConnectedStates[ev] = NULL;
    }

    // Check if state is a top level state.
    // If yes then there is no parent to it and it is also a starting
    // state when machine starts. Hence resides at StateStack[0] and we
    // set CurrStateIndex to 0.
    if (state->parent == NULL)
    {
        sm->CurrStateIndex = 0;
        sm->StateStack[sm->CurrStateIndex] = state;
    }
}

void addTransition(State_t *fromState, UINT16_t evId, State_t *toState)
{
    fromState->ConnectedStates[evId] = toState;
}

void setInitialState(State_t *state)
{
    // Mark this state as a EV_DEFAULT_ENTRY transition in its parent.
    state->parent->ConnectedStates[EV_DEFAULT_ENTRY_LIB] = state;
}

void runEntryProcedure( StateMachine_t *sm )
{
    State_t *currState = sm->StateStack[sm->CurrStateIndex];

    while (1)
    {
        Event_t ev;
        currState->entry(ev);

        currState = sm->StateStack[sm->CurrStateIndex]->ConnectedStates[EV_DEFAULT_ENTRY_LIB];

        if (NULL != currState)
        {
            sm->CurrStateIndex++;
            sm->StateStack[sm->CurrStateIndex] = currState;
        }
        else
        {
            break;
        }
    }
}

void runExitProcedure( StateMachine_t *sm, State_t *state )
{
    State_t *currState = sm->StateStack[sm->CurrStateIndex];

    Event_t ev;
    while (currState != state)
    {
        currState->exit(ev);
        sm->CurrStateIndex--;
        currState = sm->StateStack[sm->CurrStateIndex];
    }

    currState->exit(ev);
    sm->CurrStateIndex--;
}

State_t *getParentThatHandlesEvent(StateMachine_t *sm, UINT16_t eventId)
{
    State_t *currState = sm->StateStack[sm->CurrStateIndex];

    while (NULL != currState)
    {
        if (NULL != currState->ConnectedStates[eventId])
        {
            return currState;
        }
        currState = currState->parent;
    }
    return NULL;
}

void startMachine( StateMachine_t *sm )
{
    runEntryProcedure(sm);
}

void triggerEvent(StateMachine_t *sm, Event_t ev)
{
    State_t *nextState = sm->StateStack[sm->CurrStateIndex]->ConnectedStates[ev.eventId];

    // Check if current state handles this event.
    if (NULL != nextState)
    {
        // Call current states exit.
        sm->StateStack[sm->CurrStateIndex]->exit(ev);
        sm->StateStack[sm->CurrStateIndex] = nextState;

        runEntryProcedure(sm);
        return;
    }
    else
    {
        State_t *state = getParentThatHandlesEvent(sm, ev.eventId);
        if (NULL != state)
        {
            runExitProcedure(sm, state);

            sm->CurrStateIndex++;
            sm->StateStack[sm->CurrStateIndex] = state->ConnectedStates[ev.eventId];

            runEntryProcedure(sm);
            return;
        }
    }

    // If we reach here then we have to run the inprogress routines of all the states
    // in the stack till root to see if any state handles this event.
    State_t *currState = sm->StateStack[sm->CurrStateIndex];

    while (NULL != currState)
    {
        if (EVENT_HANDLED == currState->inprogress(ev))
        {
            return;
        }
        currState = currState->parent;
    }
}
