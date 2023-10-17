/// This is a autogenerated file by statemachine framework and should not be edited manually."

#include "MotorMachine.h"

/// Create state objects.
State MotorMachine("MotorMachine");
State PowerOffSt("PowerOffSt");
State PowerOnSt("PowerOnSt");
State ErrorSt("ErrorSt");
State MaintainenceSt("MaintainenceSt");
State InitializeSt("InitializeSt");
State ReadySt("ReadySt");
State RunningSt("RunningSt");
State StoppingSt("StoppingSt");
State AMainainSt("AMainainSt");
State BMainainSt("BMainainSt");


/// Create statemachine object.
StateMachine StateMachine_;

int MotorMachine::initialize(void)
{
    /// Add states to statemachine.
    StateMachine_.addState(&MotorMachine, NULL);
    StateMachine_.addState(&PowerOffSt, &MotorMachine);
    StateMachine_.addState(&PowerOnSt, &MotorMachine);
    StateMachine_.addState(&ErrorSt, &MotorMachine);
    StateMachine_.addState(&MaintainenceSt, &MotorMachine);
    StateMachine_.addState(&InitializeSt, &PowerOnSt);
    StateMachine_.addState(&ReadySt, &PowerOnSt);
    StateMachine_.addState(&RunningSt, &PowerOnSt);
    StateMachine_.addState(&StoppingSt, &PowerOnSt);
    StateMachine_.addState(&AMainainSt, &MaintainenceSt);
    StateMachine_.addState(&BMainainSt, &MaintainenceSt);


    /// Add transitions.
    PowerOffSt.addTransition(EVENT::EV_ON, &PowerOnSt);
    PowerOnSt.addTransition(EVENT::EV_ERROR, &ErrorSt);
    MaintainenceSt.addTransition(EVENT::EV_OK, &ReadySt);
    InitializeSt.addTransition(EVENT::EV_READY, &ReadySt);
    ReadySt.addTransition(EVENT::EV_RUN, &RunningSt);
    ReadySt.addTransition(EVENT::EV_MAINTAIN, &AMainainSt);
    RunningSt.addTransition(EVENT::EV_STOP, &StoppingSt);
    StoppingSt.addTransition(EVENT::EV_STOPPED, &ReadySt);
    AMainainSt.addTransition(EVENT::EV_A_B, &BMainainSt);
    BMainainSt.addTransition(EVENT::EV_RUN, &RunningSt);


    /// Set initial states.
    StateMachine_.setRootState(&MotorMachine);
    MotorMachine.setInitialChild(&PowerOffSt);
    PowerOnSt.setInitialChild(&InitializeSt);
}

int MotorMachine::start(void)
{
    StateMachine_.start();
}

int MotorMachine::triggerEv(uint32_t ev, void* msg)
{
    StateMachine_.triggerEvent(ev, msg);
}
