#include "ACStates.h"


//-------------------------------------------------------------------------------------------------------------------/
/// class State implementation.

#define DEBUG_PRINT

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
#ifdef DEBUG_PRINT
    printf("%s:entry\n", this->mName.c_str());
#endif
}


State::EventHandlingStatus_t State::run(uint32_t ev, void* msg)
{
#ifdef DEBUG_PRINT
    printf("%s:run:%d\n", this->mName.c_str(), ev);
#endif
    return EventHandlingStatus_t::EVENT_NOT_HANDLED;
}


void State::exit(uint32_t ev, void* msg)
{
#ifdef DEBUG_PRINT
    printf("%s:exit\n", this->mName.c_str());
#endif
}


void State::addTransition(uint32_t ev, State* destState)
{
    mStateTransition[ev] = destState;
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


State::EventHandlingStatus_t StateMachine::triggerEvent(uint32_t ev, void* msg)
{
    State* destState = mCurrState->mStateTransition[ev];

    /// Current state does not handle the event.
    /// Look for a state upwards that handles it.
    if (NULL == destState)
    {
        State *curr = mStateMap[mCurrState];

        /// Go upwards till the root state.
        while (NULL != curr)
        {
            /// Check if curr state handles the event in context.
            destState = curr->mStateTransition[ev];

            if (NULL != destState)  ///< curr state handles this event.
            {
                /// Perform exit procedure.
                /// We have to exit all the states in the stack till
                /// the state 'curr' which handles this event.
                while (mCurrState != curr)
                {
                    mCurrState->exit(ev, msg);
                    mCurrState = mStateMap[mCurrState];
                }

                /// Perform entry procedure if curr state
                /// and dest state has same parent.
                if (mStateMap[mCurrState] == mStateMap[destState])
                {
                    mCurrState->entry(ev, msg);
                    while (NULL != mCurrState->getInitialChild())
                    {
                        mCurrState = mCurrState->getInitialChild();
                        mCurrState->entry(ev, msg);
                    }
                    return State::EVENT_HANDLED;
                }

                /// This is a case where curr state and dest state has different parents.
                /// 1. Find a common parent.
                /// 2. Run exit procedure till common parent (do not exit common parent).
                /// 3. Call entry of each state till dest state (do not call entry of common parent).
                State* commonParent = NULL;
                State* destParent = mStateMap[destState];
                std::vector<State*> statesToEnter;

                while (NULL != destParent)
                {
                    State* currParent = mStateMap[mCurrState];
                    while (NULL != currParent)
                    {
                        if (currParent == destParent)
                        {
                            commonParent = destParent;
                            break;
                        }
                        currParent = mStateMap[currParent];
                    }
                    statesToEnter.push_back(destParent);
                    destParent = mStateMap[destParent];
                }

                if (NULL != commonParent)
                {
                    /// Perform exit procedure.
                    while (mCurrState != commonParent)
                    {
                        mCurrState->exit(ev, msg);
                        mCurrState = mStateMap[mCurrState];
                    }

                    /// Call entry of each state till dest state.
                    while (statesToEnter.size() > 0)
                    {
                        mCurrState = statesToEnter.back();
                        /// We dont want to call entry function of common parent.
                        if (commonParent != mCurrState) mCurrState->entry(ev, msg);
                        statesToEnter.pop_back();
                    }

                    mCurrState = destState;

                    /// Perform entry procedure.
                    mCurrState->entry(ev, msg);
                    while (NULL != mCurrState->getInitialChild())
                    {
                        mCurrState = mCurrState->getInitialChild();
                        mCurrState->entry(ev, msg);
                    }
                    return State::EVENT_HANDLED;
                }
            }

            /// Curr state does not handle this event. Continue to parent.
            else
            {
                curr = mStateMap[curr];
            }
        }
    }

    /// Current state handles the event.
    else
    {
        /// Check if the dest state is self state(loopback)
        /// If yes, then in this case we need to exit and enter the mCurrState.
        if (mCurrState == destState)
        {
            mCurrState->exit(ev, msg);
            mCurrState->entry(ev, msg);

            /// We can return here.
            return State::EVENT_HANDLED;
        }


        /// Check if dest state has same parent as mCurrState.
        /// If so, then exit the mCurrState and enter dest state.
        if (mStateMap[mCurrState] == mStateMap[destState])
        {
            mCurrState->exit(ev, msg);
            mCurrState = destState;
            mCurrState->entry(ev, msg);

            /// Perform entry procedure.
            while (NULL != mCurrState->getInitialChild())
            {
                mCurrState = mCurrState->getInitialChild();
                mCurrState->entry(ev, msg);
            }
            return State::EVENT_HANDLED;
        }


        /// This is a case where curr state and dest state has different parents.
        /// 1. Find a common parent.
        /// 2. Run exit procedure till common parent (do not exit common parent).
        /// 3. Call entry of each state till dest state (do not call entry of common parent).
        State* commonParent = NULL;
        State* destParent = mStateMap[destState];
        std::vector<State*> statesToEnter;

        while (NULL != destParent)
        {
            State* currParent = mStateMap[mCurrState];
            while (NULL != currParent)
            {
                if (currParent == destParent)
                {
                    commonParent = destParent;
                    break;
                }
                currParent = mStateMap[currParent];
            }
            statesToEnter.push_back(destParent);
            destParent = mStateMap[destParent];
        }

        if (NULL != commonParent)
        {
            /// Perform exit procedure.
            while (mCurrState != commonParent)
            {
                mCurrState->exit(ev, msg);
                mCurrState = mStateMap[mCurrState];
            }

            /// Call entry of each state till dest state.
            while (statesToEnter.size() > 0)
            {
                mCurrState = statesToEnter.back();
                /// We dont want to call entry function of common parent.
                if (commonParent != mCurrState) mCurrState->entry(ev, msg);
                statesToEnter.pop_back();
            }

            mCurrState = destState;

            /// Perform entry procedure.
            mCurrState->entry(ev, msg);
            while (NULL != mCurrState->getInitialChild())
            {
                mCurrState = mCurrState->getInitialChild();
                mCurrState->entry(ev, msg);
            }
            return State::EVENT_HANDLED;
        }
    }


    /// This is expected to be an internal transition.
    /// We will call "run" function of all the states in context till
    /// any of the state returns EVENT_HANDLED.
    State *curr = mCurrState;
    while (1)
    {
        if (State::EVENT_HANDLED == curr->run(ev, msg))
        {
            return State::EVENT_HANDLED;
        }

        curr = mStateMap[curr];
        if (NULL == curr)
        {
            return State::EVENT_NOT_HANDLED;    // We have reached the root state and have not found any state handling this event.
        }
    }

    // We will never come here.
    return State::EVENT_NOT_HANDLED;
}
