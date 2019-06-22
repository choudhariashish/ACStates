#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include<iostream>
#include <map> 


class EventCode;
class Transition;
class State;
class StateMachine;


enum EVENT
{
    CHANGE_INTENSITY,
    TURN_ON,
    TURN_OFF,
    ABORT
};


class TransitionCode
{
public:

    TransitionCode()
    {
	}

    virtual int execute()
    {
    }

};


class Transition
{
public:

    Transition(TransitionCode *tc, State *state)
    {
        mTransitionCode = tc;
        mState = state;
    }

    TransitionCode *mTransitionCode;
    State *mState;
};


class State
{

public:

    State(std::string name)
    {
        mName = name;
	}

    virtual void entry()
    {
        printf("%s entered\n", mName.c_str());
    }

    virtual void exit()
    {
        printf("%s exited\n", mName.c_str());
    }

    void addTransition(EVENT ev, TransitionCode *tc, State *state)
    {
        mStateTransition[ev] = new Transition(tc, state);
    }


    std::map<EVENT, Transition*> mStateTransition;

    std::string mName;
};


class StateMachine
{
public:

    void setInitialState(State *state)
    {
        mCurrState = state;
	}

    State *getCurrentState()
    {
        return mCurrState;
	}

    void start()
    {
        mCurrState->entry();
    }

    void addState(State *state, State *parent=NULL)
    {
        mStateMap[state] = parent;
	}

    void triggerEvent(EVENT ev)
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
                    int stateChangeNeeded = 0;
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
            if (NULL != transition->mTransitionCode)
            {
				if (1==transition->mTransitionCode->execute())
                {
                    if ( NULL != transition->mState )
                    {
		                mCurrState->exit();
                        transition->mState->entry();
                        mCurrState = transition->mState;
                    }
                }
            }
            else if ( NULL != transition->mState )
            {
                mCurrState->exit();
                transition->mState->entry();
                mCurrState = transition->mState;
            }
            return;
        }
    }

private:

    State *mCurrState;
    std::map<State*, State*> mStateMap;
};

#endif
