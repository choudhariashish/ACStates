#include<iostream>
#include "ACStates.h"


class TransitionCodeChangeIntensity : public TransitionCode
{
public:

    int execute()
    {
        printf("TransitionCodeChangeIntensity execution\n");
        return 0;
    }
};



int main()
{
    State *StBulb = new State("StBulb");
    State *StOn = new State("StOn");
    State *StOff = new State("StOff");

    StOff->addTransition(EVENT::TURN_ON, NULL, StOn);
    StOn->addTransition(EVENT::TURN_OFF, NULL, StOff);
    StBulb->addTransition(EVENT::ABORT, NULL, StOff);

    TransitionCodeChangeIntensity transitionCodeChangeIntensity;
    StOn->addTransition(EVENT::CHANGE_INTENSITY, &transitionCodeChangeIntensity, StOff);


    StateMachine stateMachine;

    stateMachine.addState(StBulb);
    stateMachine.addState(StOn, StBulb);
    stateMachine.addState(StOff, StBulb);

    stateMachine.setInitialState(StOff);
    stateMachine.start();

    stateMachine.triggerEvent(TURN_ON);

    stateMachine.triggerEvent(CHANGE_INTENSITY);

    stateMachine.triggerEvent(ABORT);


    return 0;
}
