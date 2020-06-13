#!/bin/bash

echo '-------------------------------------------'
echo 'Parsing Scxml Model Files------------------'
echo '-------------------------------------------'

echo 'ex1.scxml----------------------------------'
python3 ../../sc_qt_cleanup.py ../ex1.scxml
echo 'ex2.scxml----------------------------------'
python3 ../../sc_qt_cleanup.py ../ex2.scxml
echo 'ex3.scxml----------------------------------'
python3 ../../sc_qt_cleanup.py ../ex3.scxml


echo '-------------------------------------------'
echo 'Building Machine Implementations-----------'
echo '-------------------------------------------'

echo 'mainex1.c----------------------------------'
gcc -g mainex1.c -o ex1.out ../*.c -I ../
echo 'mainex2.c----------------------------------'
gcc -g mainex2.c -o ex2.out ../*.c -I ../
echo 'mainex3.c----------------------------------'
gcc -g mainex3.c -o ex3.out ../*.c -I ../



echo '-------------------------------------------'
echo 'Running Machines---------------------------'
echo '-------------------------------------------'

echo 'ex1.out------------------------------------'
./ex1.out
echo 'ex2.out------------------------------------'
./ex2.out
echo 'ex3.out------------------------------------'
./ex3.out
