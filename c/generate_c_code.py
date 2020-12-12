#!/usr/bin/python3

import os, sys
import argparse
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
        self.internal = []          # (type, event, code)
        self.external = []          # (type, event, target)
        self.entry = (False, '')    # (True/False, code)
        self.exit  = (False, '')    # (True/False, code)


#---------------------------------------------------------------
# class SMCode
#---------------------------------------------------------------
class SMCode:
    def __init__(self):
        self.includes = []
        self.states = []
        self.eventDict = dict()
        self.initialDict = dict()

    def printStates(self):
        for state in self.states:
            print(state.sid, state.internal, state.external)

    def generateCode(self):
        impl = []
        api = []
        api_sm_include = ''


        impl.append("// Note: This file is an autogenerated API of statemachine")
        impl.append("// and should not be edited manually unless you know what you are doing.\n\n")

        impl.append("#include "+'"SM'+self.states[0].sid+'.h"')
        for include in self.includes:
            if 'ACStates.h' in include:
                api_sm_include = include
            else:
                impl.append("#include "+ include)
        impl.append('\n')


        impl.append('ScheduleEventPtr_t scheduleEvent;');
        impl.append('\n')


        impl.append('enum EVENTS')
        impl.append('{')
        impl.append(4*' '+'EV_DEFAULT_ENTRY'+8*' '+'= EV_DEFAULT_ENTRY_LIB,')
        for event in self.eventDict.keys():
            impl.append(4*' '+event+',')
        impl.append('};')
        impl.append('\n')


        sids = []
        for state in self.states:
            sids.append(state.sid)
        impl.append('State_t ' + ', '.join(sids) + ';')
        impl.append('StateMachine_t sm;')
        impl.append('\n')


        for state in self.states:
            sid = state.sid
            impl.append('// '+sid)
            impl.append('EventState_t '+sid+'_entry(Event_t ev)')
            impl.append('{')
            if state.entry[0] == True:
                impl.append(4*' '+state.entry[1].replace('\n', '\n'+4*' '))
            impl.append('}')

            impl.append('EventState_t '+sid+'_inprogress(Event_t ev)')
            impl.append('{')

            for inttran in state.internal:
                impl.append(4*' '+'if (ev.eventId=='+inttran[1]+')')
                impl.append(4*' '+'{')
                code = inttran[2]
                impl.append(8*' '+code.replace('\n', '\n'+8*' '))
                impl.append(8*' '+'return EVENT_HANDLED;')
                impl.append(4*' '+'}\n')

            impl.append(4*' '+'return EVENT_NOT_HANDLED;')
            impl.append('}')

            impl.append('EventState_t '+sid+'_exit(Event_t ev)')
            impl.append('{')
            if state.exit[0] == True:
                impl.append(4*' '+state.exit[1].replace('\n', '\n'+4*' '))
            impl.append('}')
            impl.append('\n')


        impl.append('void initSM_' + self.states[0].sid+'()')
        impl.append('{')
        state_count = 0
        for state in self.states:
            if state_count==0:
                impl.append(4*' '+'prepareState(&sm, &'+state.sid+', '+state.sid+'_entry, '+state.sid+'_inprogress, '+state.sid+'_exit, NULL);')
            else:
                impl.append(4*' '+'prepareState(&sm, &'+state.sid+', '+state.sid+'_entry, '+state.sid+'_inprogress, '+state.sid+'_exit, &'+state.parent.sid+');')
            state_count += 1
        impl.append('\n')


        for state in self.states:
            for exttran in state.external:
                impl.append(4*' '+'addTransition(&'+state.sid+', '+exttran[1]+', &'+exttran[2]+');')
        impl.append('\n')


        for state in self.states:
            if state.isInitial==True:
                impl.append(4*' '+'setInitialState(&'+state.sid+');')
        impl.append('}')
        impl.append('\n')


        impl.append('void installSM_' + self.states[0].sid+'ScheduleEventHandle(ScheduleEventPtr_t ptr)')
        impl.append('{')
        impl.append(4*' '+'scheduleEvent = ptr;')
        impl.append('}')
        impl.append('\n')

        impl.append('void startSM_' + self.states[0].sid+'()')
        impl.append('{')
        impl.append(4*' '+'startMachine(&sm);')
        impl.append('}')
        impl.append('\n')

        impl.append('void triggerSM_' + self.states[0].sid+'(Event_t ev)')
        impl.append('{')
        impl.append(4*' '+'triggerEvent(&sm, ev);')
        impl.append('}')




        api.append("// Note: This file is an autogenerated implementation of statemachine")
        api.append("// and should not be edited manually unless you know what you are doing.\n\n")

        api.append("#ifndef SM_"+self.states[0].sid.upper()+"_H")
        api.append("#define SM_"+self.states[0].sid.upper()+"_H")


        api.append("#include "+ api_sm_include)


        api.append(4 * '\n')
        api.append("// Install callback handle to schedule a delayed event.")
        api.append('typedef void (*ScheduleEventPtr_t)(Event_t);')
        api.append('void installSM_' + self.states[0].sid+'ScheduleEventHandle(ScheduleEventPtr_t ptr);')

        api.append(2 * '\n')
        api.append("// Initilizes statemachine.")
        api.append('void initSM_' + self.states[0].sid+'();')

        api.append(2 * '\n')
        api.append("// Starts statemachine.")
        api.append('void startSM_' + self.states[0].sid+'();')

        api.append(2 * '\n')
        api.append("// Trigger event in statemachine.")
        api.append('void triggerSM_' + self.states[0].sid+'Event(Event_t ev);')

        api.append(4 * '\n')
        api.append("#endif    // " + "SM_"+self.states[0].sid.upper()+"_H")

        return (impl, api)


#---------------------------------------------------------------
# ParseState
#---------------------------------------------------------------
def ParseMeta(root_state, smcode):
    for node in root_state:
        if 'metadata' in node.tag:
            for meta in node:
                if 'include' in meta.tag:
                    smcode.includes = meta.text.split('\n')


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
            code = ""
            for script in node:
                if 'script' in script.tag:
                    code = script.text
            internals.append( (node.attrib['type'], node.attrib['event'], code) )

    # External transitions.
    for node in curr_state:
        if 'transition' in node.tag and node.attrib['type'] == 'external':
            externals.append( (node.attrib['type'], node.attrib['event'], node.attrib['target']) )

    # Sort transitions by events.
    internals.sort(key = lambda x: x[1])
    externals.sort(key = lambda x: x[1])

    # Create transitions and add to the current state.
    for internal in internals:
        new_state.internal.append((internal[0], internal[1], internal[2]))
        smcode.eventDict[internal[1]] = 1

    for external in externals:
        new_state.external.append((external[0], external[1], external[2]))
        smcode.eventDict[external[1]] = 1

    # Entry/Exit
    for node in curr_state:
        if 'onentry' in node.tag:
            for script in node:
                if 'script' in script.tag:
                    new_state.entry = (True, script.text)
        if 'onexit' in node.tag:
            for script in node:
                if 'script' in script.tag:
                    new_state.exit = (True, script.text)

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
    ParseMeta(statemachine, smcode)
    ParseState(statemachine, None, smcode)
    code = smcode.generateCode()

    return (statemachine.attrib['id'], code[0], code[1])


#---------------------------------------------------------------
# Main
#---------------------------------------------------------------
if __name__=="__main__":

    # Prepare argument parser.
    parser = argparse.ArgumentParser()

    parser.add_argument('--scxml',
                        action='store',
                        default='',
                        help='specify file saved by qt designer with scxml extension')

    parser.add_argument('-g',
                        '--generate',
                        action='store_true',
                        help='generate source *.c *.h files')

    arguments = parser.parse_args()

    if arguments.scxml == '':
        print("Please specify scxml file")
        sys.exit(0)

    code = ParseScxml(arguments.scxml)

    if arguments.generate == True:
        with open('SM'+code[0]+'.c', 'w') as filehandle:
            filehandle.writelines("%s\n" % line for line in code[1])
        with open('SM'+code[0]+'.h', 'w') as filehandle:
            filehandle.writelines("%s\n" % line for line in code[2])
    else:
        for line in code[1]:
            print(line)
        print(5 * '\n')
        for line in code[2]:
            print(line)
