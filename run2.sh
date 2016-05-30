#!/bin/bash 

for i in {1..10000}
do
    echo "---------------------------------"
    ./main  -l Lvls/lvl0.txt -n 4 -p player1 player1 plannerK2 plannerK2 -seed $i -f 
    echo "---------------------------------"
done
