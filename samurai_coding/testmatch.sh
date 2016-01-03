#!/bin/sh
manager1.1/gameManager \
    -a players/greedyPlayer -p "" -u "" -n "greedy1" -r 1 -s 100 \
    -a players/greedyPlayer -p "" -u "" -n "greedy2" -r 2 -s 98 \
    -a players/greedyPlayer -p "" -u "" -n "greedy3" -r 3 -s 80 \
    -a players/JinoAI -p "" -u "" -n "jinopapo4" -r 4 -s 70 \
    -a players/JinoAI -p "" -u "" -n "jinopapo5" -r 5 -s 60 \
    -a players/JinoAI  -p "" -u "" -n "jinopapo6" -r 6 -s 20 \
    -t
