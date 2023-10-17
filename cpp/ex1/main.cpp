#include "ACStates.h"

/// Declare events.
enum EVENT
{
    EV_ON,
    EV_READY,
    EV_RUN,
    EV_STOP,
    EV_STOPPED,
    EV_ERROR,
    EV_MAINTAIN,
    EV_OK,
    EV_A_B,
    EV_MONITOR_TEMP
};


/// Create state objects.
State MotorMachine("MotorMachine");
State PowerOffSt("PowerOffSt");
State PowerOnSt("PowerOnSt");
State InitializeSt("InitializeSt");
State ReadySt("ReadySt");
State RunningSt("RunningSt");
State StoppingSt("StoppingSt");
State ErrorSt("ErrorSt");
State MaintainenceSt("MaintainenceSt");
State AMaintainSt("AMaintainSt");
State BMaintainSt("BMaintainSt");

/// Create statemachine.
StateMachine stateMachine;


int main()
{
    /// Add states to statemachine.
    stateMachine.addState(&MotorMachine);
    stateMachine.addState(&PowerOffSt, &MotorMachine);
    stateMachine.addState(&PowerOnSt, &MotorMachine);
    stateMachine.addState(&InitializeSt, &PowerOnSt);
    stateMachine.addState(&ReadySt, &PowerOnSt);
    stateMachine.addState(&RunningSt, &PowerOnSt);
    stateMachine.addState(&StoppingSt, &PowerOnSt);
    stateMachine.addState(&ErrorSt, &MotorMachine);
    stateMachine.addState(&MaintainenceSt, &MotorMachine);
    stateMachine.addState(&AMaintainSt, &MaintainenceSt);
    stateMachine.addState(&BMaintainSt, &MaintainenceSt);

    /// Add transitions.
    PowerOffSt.addTransition(EVENT::EV_ON, &PowerOnSt);
    InitializeSt.addTransition(EVENT::EV_READY, &ReadySt);
    ReadySt.addTransition(EVENT::EV_RUN, &RunningSt);
    RunningSt.addTransition(EVENT::EV_STOP, &StoppingSt);
    StoppingSt.addTransition(EVENT::EV_STOPPED, &ReadySt);
    PowerOnSt.addTransition(EVENT::EV_ERROR, &ErrorSt);
    ReadySt.addTransition(EVENT::EV_MAINTAIN, &AMaintainSt);
    AMaintainSt.addTransition(EVENT::EV_A_B, &BMaintainSt);
    BMaintainSt.addTransition(EVENT::EV_RUN, &RunningSt);
    MaintainenceSt.addTransition(EVENT::EV_OK, &ReadySt);

    /// Set initial states.
    stateMachine.setRootState(&MotorMachine);
    MotorMachine.setInitialChild(&PowerOffSt);
    PowerOnSt.setInitialChild(&InitializeSt);

    /// Start statemachine.
    stateMachine.start();

    /// Trigger events to test transitions.
    stateMachine.triggerEvent(EV_ON);
    stateMachine.triggerEvent(EV_READY);
    stateMachine.triggerEvent(EV_MONITOR_TEMP);
    stateMachine.triggerEvent(EV_MAINTAIN);
    stateMachine.triggerEvent(EV_A_B);
    stateMachine.triggerEvent(EV_RUN);

    return 0;
}