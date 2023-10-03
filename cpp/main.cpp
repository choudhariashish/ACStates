///
///    model_1.qm
///
#include "ACStates.h"

#include <queue>


// A simple borrowed library for serializing states.
#include "simple_json.h"


/// Declare events.
enum EVENT
{
    TRIG1,
    TRIG2,
    TRIG3,
    TRIG4,
    TRIG5
};

const char* EventString[] =
{
    "TRIG1",
    "TRIG2",
    "TRIG3",
    "TRIG4",
    "TRIG5"
};


/// Create state objects.
State StateRoot("stateRoot");
State State1("state1");
State State2("state2");
State State3("state3");
State State4("state4");


/// Create internal transitions.
Transition Trig3;

/// Create external transitions.
Transition State2_State3_Trig1(NULL, &State3);
Transition State3_State2_Trig2(NULL, &State2);
Transition State1_State4_Trig4(NULL, &State4);
Transition State4_State1_Trig5(NULL, &State1);

/// Create guarded transitions.


/// Create statemachine.
StateMachine stateMachine;

int main()
{
    /// Add transitions.
    State2.addTransition(EVENT::TRIG1, &State2_State3_Trig1);
    State3.addTransition(EVENT::TRIG2, &State3_State2_Trig2);
    State1.addTransition(EVENT::TRIG4, &State1_State4_Trig4);
    State4.addTransition(EVENT::TRIG5, &State4_State1_Trig5);
    State1.addTransition(EVENT::TRIG3, &Trig3);

    /// Add states to statemachine.
    stateMachine.addState(&StateRoot);
    stateMachine.addState(&State1, &StateRoot);
    stateMachine.addState(&State4, &StateRoot);
    stateMachine.addState(&State2, &State1);
    stateMachine.addState(&State3, &State1);

    /// Set initial state of statemachine.
    stateMachine.setRootState(&StateRoot);
    StateRoot.setInitialChild(&State1);
    State1.setInitialChild(&State2);

    stateMachine.start();

    /// Trigger events to test transitions.
    stateMachine.triggerEvent(TRIG1);
    stateMachine.triggerEvent(TRIG2);

    json::JSON obj;
    obj["statechart"] = json::Object();
 
    auto statemap = stateMachine.getStateMap();
    std::queue<State*> stateq;
    stateq.push(stateMachine.getRootState());

    while (stateq.size() > 0)
    {
        State* s = stateq.front();
        stateq.pop();

        json::JSON state;
        std::string name = s->mName;

        state[name.c_str()]["initial"] = (NULL == s->mInitialChild) ? "None" : s->mInitialChild->mName.c_str();
        state[name.c_str()]["internals"];
        state[name.c_str()]["externals"];

        for (auto transition : s->mStateTransition)
        {
            if (NULL == transition.second->mState)
            {
              state[name.c_str()]["internals"].append(EventString[transition.first]);
            }
            else if (NULL != transition.second->mState)
            {
              state[name.c_str()]["externals"].append((std::string(EventString[transition.first])
                                                         + " -> "
                                                         + transition.second->mState->mName).c_str());
            }
        }

        obj["statechart"]["states"].append(state);

        for (auto child : s->mChildStates)
        {
            stateq.push(child);
        }
    }

    std::cout << obj << std::endl;

    return 0;
}
