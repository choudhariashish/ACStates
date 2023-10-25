//-------------------------------------------------------------------------------------------------------------------/
/// StateMachine library.
/// A quick, simple, lightweight statemachine library in C++ with dependancy
/// on only std library (string, vector, map).

#ifndef ACSTATES_H
#define ACSTATES_H

#include <string>
#include <vector>
#include <map>


//-------------------------------------------------------------------------------------------------------------------/
/// Forward declaration.
class State;
class StateMachine;


//-------------------------------------------------------------------------------------------------------------------/
/// Base class for State.
class State
{

public:

    /// Constructor.
    /// @param name name of the state.
    State(std::string name);

    /// Sets initial/default state.
    /// @param state state to transition to on entry.
    void setInitialChild(State *state);

    /// Returns initial child state.
    /// @param state initial child state.
    State* getInitialChild();

    /// Gets called after this state is entered.
    /// @param ev event that triggered this entry.
    /// @param msg user specific message.
    virtual void entry(uint32_t ev, void* msg=NULL);

    /// @brief Status to represent if an event was handled or no by the state.
    enum EventHandlingStatus_t { EVENT_HANDLED, EVENT_NOT_HANDLED };

    /// Gets called when this event is not causing any state change in the current state stack.
    /// This event is expected to be an internal event to the state. If the state in context did not
    /// handle this event, then this api should return EV_NOT_HANDLED so that it can be propagated to
    /// the parent and eventually till the root state.
    /// @param ev event that should be handled by this state.
    /// @param msg user specific message.
    virtual State::EventHandlingStatus_t run(uint32_t ev, void* msg=NULL);

    /// Gets called before this state is exited.
    /// @param ev event that triggered this exit.
    /// @param msg user specific message.
    virtual void exit(uint32_t ev, void* msg=NULL);

    /// Adds a transition for event.
    /// @param ev event enum.
    /// @param destState destination state.
    void addTransition(uint32_t ev, State* destState);

    ///        event    transition
    std::map<uint32_t, State*> mStateTransition;
    std::string mName;
    State *mInitialChild;
    std::vector<State*> mChildStates;
};


//-------------------------------------------------------------------------------------------------------------------/
/// StateMachine class
class StateMachine
{
public:

    /// Sets root state of statemachine.
    /// @param state initial state.
    void setRootState(State *state);

    /// Returns root state of statemachine.
    /// @param state initial state.
    State* getRootState();

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
    /// @param msg user specific message.
    /// @return State::EventHandlingStatus_t if event was handled or not.
    State::EventHandlingStatus_t triggerEvent(uint32_t ev, void* msg=NULL);

    /// Events that are used by statemachine internally.
    enum InternalEvent_t { NO_OP_EVENT };

    /// Return statemap which is useful to serialize statechart.
    std::map<State*, State*> getStateMap() { return mStateMap; };

private:

    State *mCurrState;
    State *mRootState;

    ///     current  parent
    std::map<State*, State*> mStateMap;
};

#endif    /* ACSTATES_H */
