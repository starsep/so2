#!/bin/bash

length=4
depth=3
F=3
S=5
A=11

./muzeum $length $depth $S $A < example1_muzeum &
./bank $F $S $A < example1_bank &
