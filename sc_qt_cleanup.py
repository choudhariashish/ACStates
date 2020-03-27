#!/usr/bin/python3

import os, sys
import xml.etree.ElementTree as ET

from xml.dom import minidom


#---------------------------------------------------------------
# PrettyFormat
#---------------------------------------------------------------
def PrettyFormat(root):
    """
    Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(root, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")


#---------------------------------------------------------------
# GenerateCode
#---------------------------------------------------------------
def GenerateCode(input_file):
    return


#---------------------------------------------------------------
# ParseQtScxml
#---------------------------------------------------------------
def ParseState(curr_state, root_state):

    states = []
    internals = []
    externals = []

    # Create a new state.
    new_curr_state = ET.SubElement(root_state, 'state')
    new_curr_state.attrib['id'] = curr_state.attrib['id']
    if 'initial' in curr_state.attrib:
        new_curr_state.attrib['initial'] = curr_state.attrib['initial']

    # Find all states and create a temporary list.
    for node in curr_state:
        if 'state' in node.tag:
            states.append(node)

    # Sort states by ids.
    states = sorted(states, key = lambda i: i.attrib['id']) 
    for node in states:
        # Make a recursive call.
        ParseState(node, new_curr_state)

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
        new_transition = ET.SubElement(new_curr_state, 'transition')
        new_transition.attrib['type'] = internal[0]
        new_transition.attrib['event'] = internal[1]

    for external in externals:
        new_transition = ET.SubElement(new_curr_state, 'transition')
        new_transition.attrib['type'] = external[0]
        new_transition.attrib['event'] = external[1]
        new_transition.attrib['target'] = external[2]

    return

#---------------------------------------------------------------
# ParseQtScxml
#---------------------------------------------------------------
def ParseQtScxml(input_file):

    tree = ET.parse(input_file)
    root = tree.getroot()

    # Any statemachine should always have these following
    # attributes.
    root_state = ET.Element("state")
    root_state.attrib['id'] = root.attrib['name']
    root_state.attrib['initial'] = root.attrib['initial']

    # We will not have any transition nodes at root level,
    # but only states.
    for node in root:
        if 'state' in node.tag:
            ParseState(node, root_state)

    print(PrettyFormat(root_state))
    return root_state


#---------------------------------------------------------------
# Main
#---------------------------------------------------------------
if __name__=="__main__":

    input_file = sys.argv[1]
    state_machine = ParseQtScxml(input_file)
    GenerateCode(state_machine)

