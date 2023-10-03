import os, sys
import argparse
import xml.etree.ElementTree as ET


#---------------------------------------------------------------------------------------------------------------------
class State:
    '''
    Class to hold state details.
    '''
    def __init__(self):
        self.name = 'Unknown'
        self.parent = None
        self.isComposite = False
        self.initial = None
        self.keepsHistory = False
        self.child_states = []
        self.externals = []
        self.internals = []
        self.transitions = []


#---------------------------------------------------------------------------------------------------------------------
def findTags(root_node, tag_name):
    '''
    Finds and returns charts in the node tree.
    '''
    nodes = [root_node]
    nodes_with_tag_name = []
    while len(nodes):
        node = nodes[0]
        nodes = nodes[1:]
        if tag_name == node.tag:
            nodes_with_tag_name.append(node)
        for child_node in node:
            nodes.append(child_node)
    return nodes_with_tag_name


#---------------------------------------------------------------------------------------------------------------------
def getTargetName(node_parent_dict, node):
    '''
    Returns the name of the target state from path (../../) specified in the node.
    '''
    print("--------------------------------------------")
    path = node.attrib['target']
    if 'cpref' in node.attrib.keys():
        path = node.attrib['cpref']
    level = path.count('../')                  # Levels to go up.

    path = path.replace('../', '').split('/')
    indexes = []                               # Indexes to go down.
    for index_str in path:
        indexes.append(int(index_str))

    if 'trig' in node.attrib.keys():
        print("trig,path,indexex,level:", node.attrib['trig'], node.attrib['target'], indexes, level)
    else:
        print("path,indexes,level:", node.attrib['target'], indexes, level)

    curr_node = node
    level_max = level
    while level > 0:
        curr_node = node_parent_dict[curr_node]
        print(" "*(level_max-level), "up:", curr_node.attrib['name'])
        level -= 1

    index = 0
    down_level = 0
    print_name = ''
    while len(indexes) > 0:
        index = indexes[0]

        # Entry/Exit tags are not counted in the path index. Hence offset the index.
        index_offset = 0
        while curr_node[index_offset].tag == 'entry' or curr_node[index_offset].tag == 'exit':
            index_offset += 1
        index += index_offset

        curr_node = curr_node[index]
        if 'state' == curr_node.tag:
            target_name = curr_node.attrib['name']
        elif 'history' == curr_node.tag:
            target_name = node_parent_dict[curr_node].attrib['name']
            node.attrib['cpref'] = curr_node.attrib['type']
        else:
            print("Fault, there has to be a state/history node at this index", index)
            sys.exit(0)

        print(" "*down_level, "down:", target_name)
        down_level += 1
        indexes = indexes[1:]
    return target_name


#---------------------------------------------------------------------------------------------------------------------
def parseQM(root):
    '''
    Parse QM format xml tree.
    '''
    node_parent_dict = dict()
    node_parent_dict[root] = None
    nodes = [root]

    # Create child-parent lookup dictionary.
    while len(nodes):
        curr_node = nodes[0]
        nodes = nodes[1:]

        for next_node in curr_node:
            node_parent_dict[next_node] = curr_node
            nodes.append(next_node)

    # Update target names in the QM data.
    nodes = [root]
    while len(nodes):
        curr_node = nodes[0]
        nodes = nodes[1:]

        # This is a external transition.
        if 'target' in curr_node.attrib.keys():
            target_name = getTargetName(node_parent_dict, curr_node)
            curr_node.attrib['target'] = target_name

        elif 'tran' == curr_node.tag:
            curr_node.attrib['target'] = 'Internal'

        for next_node in curr_node:
            nodes.append(next_node)

    print(ET.dump(root))

    # Prepare our state objects.
    nodes = [root]
    state_dict = {}
    while len(nodes):
        curr_node = nodes[0]
        nodes = nodes[1:]

        if 'state' == curr_node.tag:
            curr_state = State()
            curr_state.name = curr_node.attrib['name']
            state_dict[curr_state.name] = curr_state

            for entity in curr_node:
                if 'initial' == entity.tag:
                    curr_state.isComposite = True
                    curr_state.initial = entity.attrib['target']

                if 'state' == entity.tag:
                    curr_state.child_states.append(entity.attrib['name'])

                # External.
                if 'tran' == entity.tag and 'Internal' != entity.attrib['target']:
                    curr_state.transitions.append(entity.attrib['trig'] +' -> '+ entity.attrib['target'])
                    curr_state.externals.append(entity.attrib['trig'] +' -> '+ entity.attrib['target'])

                # Internal.
                if 'tran' == entity.tag and 'Internal' == entity.attrib['target']:
                    curr_state.transitions.append(entity.attrib['trig'])
                    curr_state.internals.append(entity.attrib['trig'])

        for next_node in curr_node:
            nodes.append(next_node)

    print("--------------------------------------------")
    for name_key in state_dict.keys():
        print(state_dict[name_key].name)

        print("\tinitial:")
        if state_dict[name_key].isComposite:
            print("\t\t", state_dict[name_key].initial)

        print("\tchildren:")
        for child in state_dict[name_key].child_states:
            print("\t\t", child)

        print("\texternals:")
        for tran in state_dict[name_key].externals:
            print("\t\t", tran)

        print("\tinternals:")
        for tran in state_dict[name_key].internals:
            print("\t\t", tran)


#---------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    # Prepare argument parser.
    parser = argparse.ArgumentParser()
    parser.add_argument('--qm',
                        action='store',
                        default='',
                        help='specify the file saved by qm designer')
    parser.add_argument('--sc',
                        action='store',
                        default='',
                        help='specify the statechart within the qm model')
    parser.add_argument('--sc-show',
                        action='store_true',
                        help='prints the statecharts found within the qm model')
    arguments = parser.parse_args()

    if arguments.qm == '':
        print("Please specify qm file")
        sys.exit(0)

    tree = ET.parse(arguments.qm)
    root = tree.getroot()

    charts = findTags(root, 'statechart')

    if arguments.sc_show:
        for chart in charts:
            print(chart[0].attrib['name'])
        sys.exit(0)

    if arguments.sc == '':
        print("Please specify the statechart within qm model")
        sys.exit(0)
    else:
        for chart in charts:
            if arguments.sc == chart[0].attrib['name']:
                parseQM(chart)
