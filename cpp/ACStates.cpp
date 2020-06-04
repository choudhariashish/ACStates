#include "ACStates.h"


/// class TransitionCode implementation.

TransitionCode::TransitionCode()
{
}


uint8_t TransitionCode::execute()
{
    return 0;
}


/// Transition class that is used internally
/// by statemachine.
class Transition
{
public:

    Transition(TransitionCode *tc, State *state);

    TransitionCode *mTransitionCode;
    State *mState;
};


/// class Transition implementation.


Transition::Transition(TransitionCode *tc, State *state)
{
    mTransitionCode = tc;
    mState = state;
}


/// class State implementation.

State::State(std::string name)
{
    mName = name;
    mInitialChild = NULL;
}


void State::addInitialChild(State *state)
{
    mInitialChild = state;
}


void State::entry()
{
}


void State::exit()
{
}


void State::addTransition(uint32_t ev, TransitionCode *tc, State *state)
{
    mStateTransition[ev] = new Transition(tc, state);
}


/// class StateMachine implementation.


void StateMachine::setInitialState(State *state)
{
    mCurrState = state;
}


State *StateMachine::getCurrentState()
{
    return mCurrState;
}


void StateMachine::start()
{
    mCurrState->entry();
}


void StateMachine::addState(State *state, State *parent)
{
    mStateMap[state] = parent;
}


void StateMachine::triggerEvent(uint32_t ev)
{
    Transition *transition = mCurrState->mStateTransition[ev];

    if (NULL==transition)  ///< Current state does not handle the event.
    {                      ///< Look for a state upwards that handles it.

        State *curr = mStateMap[mCurrState];
        while (NULL != curr)
        {
            /// Check if curr handles the event in context.
            transition = curr->mStateTransition[ev];

            if (NULL==transition)  ///< curr does not handle this event.
            {
                curr = mStateMap[curr];
            }
            else                   ///< curr handles this event.
            {
                uint8_t stateChangeNeeded = 0;
                if (NULL != transition->mTransitionCode)
                {
                    if (1==transition->mTransitionCode->execute())
                    {
                        stateChangeNeeded = 1;
                    }
                }

                /// Check if we need to change state.
                /// If we do, then we have to exit all the states in stack till the state
                /// 'curr' which handles this event.
                if ( (1==stateChangeNeeded && (NULL != transition->mState))
                  || (NULL==transition->mTransitionCode && (NULL != transition->mState)))
                {
                    State *stateToExit = mCurrState;
                    while (stateToExit != curr)
                    {
                        stateToExit->exit();
                        stateToExit = mStateMap[stateToExit];
                    }

                    /// Enter the target state.
                    transition->mState->entry();
                    mCurrState = transition->mState;
                    return;
                }
            }
        }
    }
    else                   ///< Current state handles the event.
    {
        uint8_t stateChangeNeeded = 0;
        if (NULL != transition->mTransitionCode)
        {
            if (1==transition->mTransitionCode->execute())
            {
                stateChangeNeeded = 1;
            }
        }
        else if ( NULL != transition->mState )
        {
            stateChangeNeeded = 1;
        }

        if (1==stateChangeNeeded)
        {
            /// Check if the state that we are about to enter is self state(loopback)
            /// If yes, then in this case we need to exit and enter the mCurrState.
            if (mCurrState==transition->mState)
            {
                mCurrState->exit();
                mCurrState->entry();

                /// We can return here.
                return;
            }

            /// Check if the state we are about to enter is mCurrState's child/subchild.
            /// If yes, then we do not have to exit this state.
            State *curr = transition->mState;

            while (curr != NULL)
            {
                if (curr==mCurrState)
                {
                    break;
                }
                curr = mStateMap[curr];
            }

            if (curr==NULL)
            {
                mCurrState->exit();
            }


            transition->mState->entry();
            mCurrState = transition->mState;

            /// Check if the state we just entered has a initial
            /// child state. If yes, then we need to enter that child too.
            while (mCurrState->mInitialChild != NULL)
            {
                mCurrState->mInitialChild->entry();
                mCurrState = mCurrState->mInitialChild;
            }
        }
    }
}
