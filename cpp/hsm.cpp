#include<iostream>
#include "ACStates.h"


/// Declare events.
enum EVENT
{
    B_C,
    D_E,
    F_G,
    G_F,
    C_G,
    C_H,
    H_B,
    F_H,
    A_G,
    G_G
};


int main()
{
    /// Create state objects.
    State *StA = new State("StA");
    State *StB = new State("StB");
    State *StC = new State("StC");
    State *StD = new State("StD");
    State *StE = new State("StE");
    State *StF = new State("StF");
    State *StG = new State("StG");
    State *StH = new State("StH");


    /// Add state transitions.
    ///
    /// addTransition(EVENT, TransitionCode*, State*)
    ///
    ///  Note: If TransitionCode.execute() returns 0
    ///        then state will not be transited.
    ///
    StB->addTransition(EVENT::B_C, NULL, StC);
    StD->addTransition(EVENT::D_E, NULL, StE);
    StF->addTransition(EVENT::F_G, NULL, StG);
    StG->addTransition(EVENT::G_F, NULL, StF);
    StC->addTransition(EVENT::C_G, NULL, StG);
    StC->addTransition(EVENT::C_H, NULL, StH);
    StH->addTransition(EVENT::H_B, NULL, StB);
    StA->addTransition(EVENT::A_G, NULL, StG);
    StG->addTransition(EVENT::G_G, NULL, StG);


    /// Add initial childs.
    StA->addInitialChild(StB);
    StC->addInitialChild(StD);
    StE->addInitialChild(StF);


    /// Create statemachine object.
    StateMachine stateMachine;


    /// Add states to statemachine.
    ///
    /// stateMachine.addState(state, parent)
    ///
    stateMachine.addState(StA);
    stateMachine.addState(StB, StA);
    stateMachine.addState(StC, StA);
    stateMachine.addState(StD, StC);
    stateMachine.addState(StE, StC);
    stateMachine.addState(StF, StE);
    stateMachine.addState(StG, StE);
    stateMachine.addState(StH, StA);


    /// Set initial state for statemachine.
    stateMachine.setInitialState(StB);


    /// Start statemachine.
    stateMachine.start();


    /// Trigger events for testing.
    stateMachine.triggerEvent(B_C);
    stateMachine.triggerEvent(D_E);
    stateMachine.triggerEvent(F_G);

    stateMachine.triggerEvent(H_B);    /// This should not get handled.

    stateMachine.triggerEvent(A_G);
    stateMachine.triggerEvent(G_G);
    return 0;
}
