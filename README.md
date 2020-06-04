# ACStates
StateMachine library. This repository contains minimalistic hierarchical statemachine library implementation. C implementation is available in c/ directory and C++ in cpp directory. To use this library include the respective ACStates.h and ACStates.cpp/c in your source. Additionally this repository also includes scipts to parse statemachine models created in QTCreator to a standard Scxml format. There are examples included in c/ and cpp/ to demostrate the usage of this library.

## Implementation in C

### Example. ex1

<p align="center"><img src="https://github.com/choudhariashish/ACStates/blob/master/c/ex1.png"></p>

The above implementation can be found in mainex1.c. This is a simple example where a top level state includes two child states named-st_idle, st_running. st_idle is set as initial state, which is, as soon as the st_machine starts the first state its going to enter is st_idle. There are two events EV_STOP and EV_RUN that allows switching between st_idle and st_running.

### Example. ex2

<p align="center"><img src="https://github.com/choudhariashish/ACStates/blob/master/c/ex2.png"></p>

The above implementation can be found in mainex2.c. This is a slight modification from example1, in the sense an additional state st_operational is introduced in between st_machine and the same two states-st_idle and st_running. As soon as the machine starts, the first state entered is st_operational as it is marked (a left-top DOT) as initial state for st_machine. The second state entered is st_idle, as again its marked as intial for st_operational.

### Example. ex3

<p align="center"><img src="https://github.com/choudhariashish/ACStates/blob/master/c/ex3.png"></p>

The above implementation can be found in mainex3.c. This is a slight modification from example2, as seen in the figure above state named st_faulted is included in st_machine at a level parallel to st_operational. There are two new events EV_FAULT and EV_FAULT_CLEAR to cause entry and exit to st_faulted state. If the control is in st_idle/st_running and EV_FAULT is triggered then st_idle/st_running will call their exit routines and also exit routine of st_operational will be called, as we can see in the figure that st_faulted is outside the state st_operational, and after that the entry routine of the st_fauled is entered. While in st_faulted if EV_FAULT_CLEAR is triggered then st_operational will be entered first and then the st_idle.
