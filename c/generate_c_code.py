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
# ParsetScxml
#---------------------------------------------------------------
def ParseScxml(input_file):

    tree = ET.parse(input_file)
    root = tree.getroot()

    print(PrettyFormat(root))


#---------------------------------------------------------------
# Main
#---------------------------------------------------------------
if __name__=="__main__":

    input_file = sys.argv[1]
    ParseScxml(input_file)