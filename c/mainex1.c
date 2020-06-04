#include "ACStates.h"

enum EVENTS
{
    EV_DEFAULT_ENTRY        = EV_DEFAULT_ENTRY_LIB,
    EV_RUN,
    EV_STOP
};


State_t st_machine, st_idle, st_running;
StateMachine_t sm;


void st_machine_entry( Event_t ev )
{
}
void st_machine_inprogress( Event_t ev )
{
}
void st_machine_exit( Event_t ev )
{
}

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
    prepareState(&sm, &st_idle, st_idle_entry, st_idle_inprogress, st_idle_exit, &st_machine);
    prepareState(&sm, &st_running, st_running_entry, st_running_inprogress, st_running_exit, &st_machine);

    addTransition(&st_idle, EV_RUN, &st_running);
    addTransition(&st_running, EV_STOP, &st_idle);

    setInitialState(&st_idle);

    startMachine(&sm);

    Event_t ev;
    ev.eventId = EV_RUN;
    triggerEvent(&sm, ev);

    ev.eventId = EV_STOP;
    triggerEvent(&sm, ev);

    return 0;
}
