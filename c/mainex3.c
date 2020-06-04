#include "ACStates.h"

enum EVENTS
{
    EV_DEFAULT_ENTRY        = EV_DEFAULT_ENTRY_LIB,
    EV_FAULT,
    EV_FAULT_CLEAR,
    EV_RUN,
    EV_STOP
};


State_t st_machine, st_faulted, st_operational, st_idle, st_running;
StateMachine_t sm;


// st_machine
void st_machine_entry( Event_t ev )
{
}
void st_machine_inprogress( Event_t ev )
{
}
void st_machine_exit( Event_t ev )
{
}

// st_faulted
void st_faulted_entry( Event_t ev )
{
    printf("st_faulted_entry\n");
}
void st_faulted_inprogress( Event_t ev )
{
}
void st_faulted_exit( Event_t ev )
{
    printf("st_faulted_exit\n");
}

// st_operational
void st_operational_entry( Event_t ev )
{
    printf("st_operational_entry\n");
}
void st_operational_inprogress( Event_t ev )
{
}
void st_operational_exit( Event_t ev )
{
    printf("st_operational_exit\n");
}

// st_idle
void st_idle_entry( Event_t ev )
{
    printf("st_idle_entry\n");
}
void st_idle_inprogress( Event_t ev )
{
}
void st_idle_exit( Event_t ev )
{
    printf("st_idle_exit\n");
}

// st_running
void st_running_entry( Event_t ev )
{
    printf("st_running_entry\n");
}
void st_running_inprogress( Event_t ev )
{
}
void st_running_exit( Event_t ev )
{
    printf("st_running_exit\n");
}

    

int main()
{
    prepareState(&sm, &st_machine, st_machine_entry, st_machine_inprogress, st_machine_exit, NULL);

    // Tree 1
    prepareState(&sm, &st_faulted, st_faulted_entry, st_faulted_inprogress, st_faulted_exit, &st_machine);

    // Tree 2
    prepareState(&sm, &st_operational, st_operational_entry, st_operational_inprogress, st_operational_exit, &st_machine);
    prepareState(&sm, &st_idle, st_idle_entry, st_idle_inprogress, st_idle_exit, &st_operational);
    prepareState(&sm, &st_running, st_running_entry, st_running_inprogress, st_running_exit, &st_operational);

    addTransition(&st_machine, EV_FAULT, &st_faulted);
    addTransition(&st_faulted, EV_FAULT_CLEAR, &st_operational);
    addTransition(&st_idle, EV_RUN, &st_running);
    addTransition(&st_running, EV_STOP, &st_idle);

    setInitialState(&st_operational);
    setInitialState(&st_idle);

    startMachine(&sm);

    Event_t ev;
    ev.eventId = EV_RUN;
    triggerEvent(&sm, ev);

    ev.eventId = EV_FAULT;
    triggerEvent(&sm, ev);

    ev.eventId = EV_STOP;
    triggerEvent(&sm, ev);

    ev.eventId = EV_FAULT_CLEAR;
    triggerEvent(&sm, ev);

    return 0;
}
