#!/usr/bin/python3

import os, sys
import xml.etree.ElementTree as ET
from xml.dom import minidom


#---------------------------------------------------------------
# class State
#---------------------------------------------------------------
class State:
    def __init__(self):
        self.sid = ''
        self.parent = []
        self.isInitial = False
        self.internal = []    # (type, event)
        self.external = []    # (type, event, target)


#---------------------------------------------------------------
# class SMCode
#---------------------------------------------------------------
class SMCode:
    def __init__(self):
        self.states = []
        self.eventDict = dict()
        self.initialDict = dict()

    def printStates(self):
        for state in self.states:
            print(state.sid, state.internal, state.external)

    def generateCode(self):
        print('#include "ACStates.h"')
        print('\n')


        print('enum EVENTS')
        print('{')
        print(4*' '+'EV_DEFAULT_ENTRY'+8*' '+'= EV_DEFAULT_ENTRY_LIB,')
        for event in self.eventDict.keys():
            print(4*' '+event+',')
        print('};')
        print('\n')


        sids = []
        for state in self.states:
            sids.append(state.sid)
        print('State_t ' + ', '.join(sids) + ';')
        print('StateMachine_t sm;')
        print('\n')


        for sid in sids:
            print('// '+sid)
            print('EventState_t '+sid+'_entry(Event_t ev)')
            print('{')
            print('}')
            print('EventState_t '+sid+'_inprogress(Event_t ev)')
            print('{')
            print('}')
            print('EventState_t '+sid+'_exit(Event_t ev)')
            print('{')
            print('}')
            print('\n')


        print('int main()')
        print('{')
        state_count = 0
        for state in self.states:
            if state_count==0:
                print(4*' '+'prepareState(&sm, &'+state.sid+', '+state.sid+'_entry, '+state.sid+'_inprogress, '+state.sid+'_exit, NULL);')
            else:
                print(4*' '+'prepareState(&sm, &'+state.sid+', '+state.sid+'_entry, '+state.sid+'_inprogress, '+state.sid+'_exit, &'+state.parent.sid+');')
            state_count += 1
        print('\n')


        for state in self.states:
            for exttran in state.external:
                print(4*' '+'addTransition(&'+state.sid+', '+exttran[1]+', &'+exttran[2]+');')
        print('\n')


        for state in self.states:
            if state.isInitial==True:
                print(4*' '+'setInitialState(&'+state.sid+');')
        print('\n')


        print(4*' '+'startMachine(&sm);')
        print('\n')


        print(4*' '+'return 0;')
        print('}')


#---------------------------------------------------------------
# ParseState
#---------------------------------------------------------------
def ParseState(curr_state, parent, smcode):

    states = []
    internals = []
    externals = []

    # Add a new state.
    new_state = State()
    smcode.states.append(new_state)

    new_state.sid = curr_state.attrib['id']
    new_state.parent = parent

    # Check if this state is marked as initial by its parent.
    if new_state.sid in smcode.initialDict.keys():
        new_state.isInitial = True


    # Check if this state has a child marked as initial for this state.
    if 'initial' in curr_state.attrib:
        smcode.initialDict[curr_state.attrib['initial']] = 1

    # Walk through all states under this state.
    for node in curr_state:
        if 'state' in node.tag:
            states.append(node)

    # Sort states by ids.
    states = sorted(states, key = lambda i: i.attrib['id'])
    for node in states:
        # Make a recursive call.
        ParseState(node, new_state, smcode)


    # Internal transitions.
    for node in curr_state:
        if 'transition' in node.tag and node.attrib['type'] == 'internal':
            internals.append( (node.attrib['type'], node.attrib['event']) )

    # External transitions.
    for node in curr_state:
        if 'transition' in node.tag and node.attrib['type'] == 'external':
            externals.append( (node.attrib['type'], node.attrib['event'], node.attrib['target']) )

    # Sort transitions by events.
    internals.sort(key = lambda x: x[1])
    externals.sort(key = lambda x: x[1])

    # Create transitions and add to the current state.
    for internal in internals:
        new_state.internal.append((internal[0], internal[1]))
        smcode.eventDict[internal[1]] = 1

    for external in externals:
        new_state.external.append((external[0], external[1], external[2]))
        smcode.eventDict[external[1]] = 1

    return


#---------------------------------------------------------------
# ParseScxml
#---------------------------------------------------------------
def ParseScxml(input_file):

    tree = ET.parse(input_file)
    root = tree.getroot()

    statemachine = None
    for child in root:
        statemachine = child

    smcode = SMCode()
    ParseState(statemachine, None, smcode)
    smcode.generateCode()


#---------------------------------------------------------------
# Main
#---------------------------------------------------------------
if __name__=="__main__":

    input_file = sys.argv[1]
    ParseScxml(input_file)
