#include "ACStates.h"


/// Declare events.
enum EVENT
{
    TURN_ON,
    CHANGE_INTENSITY,
    TURN_OFF,
    ABORT
};


/// A class that implements transition code.
class TransitionCodeChangeIntensity : public TransitionCode
{
public:

    uint8_t execute()
    {
        printf("TransitionCodeChangeIntensity execution\n");
        return 0;   ///< If we return '1' here then statemachine will
                    ///< transition to another state if the state
                    ///< was configured (state!=NULL).
    }
};


int main()
{

    /// Create state objects.
    State *StBulb = new State("StBulb");
    State *StOn = new State("StOn");
    State *StOff = new State("StOff");

    /// Add transitions.
    StOff->addTransition(EVENT::TURN_ON, NULL, StOn);
    StOn->addTransition(EVENT::TURN_OFF, NULL, StOff);
    StBulb->addTransition(EVENT::ABORT, NULL, StOff);

    /// Add a transition with code.
    TransitionCodeChangeIntensity transitionCodeChangeIntensity;
    StOn->addTransition(EVENT::CHANGE_INTENSITY, &transitionCodeChangeIntensity, StOff);

    /// Create statemachine object.
    StateMachine stateMachine;

    /// Add states to statemachine.
    stateMachine.addState(StBulb);
    stateMachine.addState(StOn, StBulb);
    stateMachine.addState(StOff, StBulb);

    /// Set initial state of statemachine.
    stateMachine.setInitialState(StOff);
    stateMachine.start();


    /// Trigger events to test transitions.
    stateMachine.triggerEvent(TURN_ON);

    stateMachine.triggerEvent(CHANGE_INTENSITY);

    stateMachine.triggerEvent(ABORT);


    return 0;
}
