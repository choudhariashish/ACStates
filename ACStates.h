/// StateMachine library.
/// A quick, simple, lightweight statemachine library in C++ with dependancy
/// on only std::map.

#ifndef ACSTATES_H
#define ACSTATES_H

#include <string>
#include <map>



/// Forward declaration.
class TransitionCode;
class Transition;
class State;
class StateMachine;


/// Base class to serve adding conditional transition code.
class TransitionCode
{
public:

    /// Constructor.
    TransitionCode();

    /// Execute the transition code.
    /// @return returns 1 if success, 0 if fail.
    virtual uint8_t execute();
};


/// Base class for State.
class State
{

public:

    /// Constructor.
    /// @param name name of the state.
    State(std::string name);

    /// Adds initial/default state.
    /// @param state state to transition to on entry.
    void addInitialChild(State *state);

    /// Gets called after this state is entered.
    virtual void entry();

    /// Gets called before this state is exited.
    virtual void exit();

    /// Adds a transition for event.
    /// @param ev event enum.
    /// @param tc transition code (specify NULL if not required).
    ///           Note: Transition to next state will happen only if
    ///           transition code evaluates to success.
    /// @param state next state (specify NULL if internal transition).
    void addTransition(uint32_t ev, TransitionCode *tc, State *state);

    ///        event    transition
    std::map<uint32_t, Transition*> mStateTransition;
    std::string mName;
    State *mInitialChild;
};


/// StateMachine class
class StateMachine
{
public:

    /// Sets intial state of statemachine.
    /// @param state initial state.
    void setInitialState(State *state);

    /// Gets the current state of statemachine.
    /// @return returns the current state.
    State *getCurrentState();

    /// Starts the statemachine execution.
    /// Statemachine should transition to initial state after start.
    void start();

    /// Adds a state to statemachine.
    /// @param state pointer to state.
    /// @param parent parent state of the state that has to be added.
    void addState(State *state, State *parent=NULL);

    /// Triggers statemachine event.
    /// @param ev event enum.
    void triggerEvent(uint32_t ev);

private:

    State *mCurrState;

    ///     current  parent
    std::map<State*, State*> mStateMap;
};

#endif    /* ACSTATES_H */
