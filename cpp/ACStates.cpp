#include "ACStates.h"


//-------------------------------------------------------------------------------------------------------------------/
/// class TransitionCode implementation.

TransitionGuard::TransitionGuard()
{
}


TransitionGuard::TransitionGuardStatus_t TransitionGuard::execute()
{
    return TransitionGuardStatus_t::GUARD_CLEAR;
}


//-------------------------------------------------------------------------------------------------------------------/
/// class Transition implementation.

Transition::Transition()
{
    mTransitionGuard = NULL;
    mState = NULL;
}


Transition::Transition(State *state)
{
    mTransitionGuard = NULL;
    mState = state;
}


Transition::Transition(TransitionGuard *tc, State *state)
{
    mTransitionGuard = tc;
    mState = state;
}


//-------------------------------------------------------------------------------------------------------------------/
/// class State implementation.

State::State(std::string name)
{
    mName = name;
    mInitialChild = NULL;
}


void State::setInitialChild(State *state)
{
    mInitialChild = state;
}


State* State::getInitialChild()
{
    return mInitialChild;
}


void State::entry(uint32_t ev, void* msg)
{
    printf("%s:entry\n", this->mName.c_str());
}


State::EventHandlingStatus_t State::run(uint32_t ev, void* msg)
{
    printf("%s:run:%d\n", this->mName.c_str(), ev);

    return EventHandlingStatus_t::EVENT_NOT_HANDLED;
}


void State::exit(uint32_t ev, void* msg)
{
    printf("%s:exit\n", this->mName.c_str());
}


void State::addTransition(uint32_t ev, Transition *transition)
{
    mStateTransition[ev] = transition;
}


//-------------------------------------------------------------------------------------------------------------------/
/// class StateMachine implementation.

void StateMachine::setRootState(State *state)
{
    mCurrState = state;
    mRootState = mCurrState;
}


State* StateMachine::getRootState()
{
    return mRootState;
}


State *StateMachine::getCurrentState()
{
    return mCurrState;
}


void StateMachine::start()
{
    /// Enter the target state.
    mCurrState->entry(StateMachine::NO_OP_EVENT, NULL);

    while (NULL != mCurrState->getInitialChild())
    {
        mCurrState = mCurrState->getInitialChild();
        mCurrState->entry(StateMachine::NO_OP_EVENT, NULL);
    }
}


void StateMachine::addState(State *state, State *parent)
{
    mStateMap[state] = parent;

    if (NULL != parent)
    {
        parent->mChildStates.push_back(state);
    }
}


void StateMachine::triggerEvent(uint32_t ev, void* msg)
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
                if (NULL != transition->mTransitionGuard)
                {
                    if (TransitionGuard::TransitionGuardStatus_t::GUARD_CLEAR==transition->mTransitionGuard->execute())
                    {
                        stateChangeNeeded = 1;
                    }
                }

                /// Check if we need to change state.
                /// If we do, then we have to exit all the states in stack till the state
                /// 'curr' which handles this event.
                if ( (1==stateChangeNeeded && (NULL != transition->mState))
                  || (NULL==transition->mTransitionGuard && (NULL != transition->mState)))
                {
                    State *stateToExit = mCurrState;
                    while (stateToExit != curr)
                    {
                        stateToExit->exit(ev, msg);
                        stateToExit = mStateMap[stateToExit];
                    }

                    mCurrState = transition->mState;
                    mCurrState->entry(ev, msg);

                    while (NULL != mCurrState->getInitialChild())
                    {
                        mCurrState = mCurrState->getInitialChild();
                        mCurrState->entry(ev, msg);
                    }
                    return;
                }
            }
        }
    }
    else                   ///< Current state handles the event.
    {
        uint8_t stateChangeNeeded = 0;
        if (NULL != transition->mTransitionGuard)
        {
            if (TransitionGuard::TransitionGuardStatus_t::GUARD_CLEAR==transition->mTransitionGuard->execute())
            {
                stateChangeNeeded = 1;
            }
        }
        else if ( NULL != transition->mState )
        {
            stateChangeNeeded = 1;
        }
        else if (NULL == transition->mTransitionGuard && NULL == transition->mState)    // This is internal transition.
        {
            State *curr = mCurrState;
            while (State::EVENT_NOT_HANDLED == curr->run(ev, msg))
            {
                curr = mStateMap[curr];
                if (NULL == curr)
                {
                    return;    // We have reached the root state.
                }
            }
            return;
        }


        if (1==stateChangeNeeded)
        {
            /// Check if the state that we are about to enter is self state(loopback)
            /// If yes, then in this case we need to exit and enter the mCurrState.
            if (mCurrState==transition->mState)
            {
                mCurrState->exit(ev, msg);
                mCurrState->entry(ev, msg);

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
                mCurrState->exit(ev, msg);
            }

            /// Enter the target state.
            mCurrState = transition->mState;
            mCurrState->entry(ev, msg);

            while (NULL != mCurrState->getInitialChild())
            {
                mCurrState = mCurrState->getInitialChild();
                mCurrState->entry(ev, msg);
            }
        }
    }
}
