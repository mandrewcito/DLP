#!/bin/bash

#cat ../test/conf.txt | valgrind ./automata
cat ../test/conf.txt | ./automata | dot -Tpng -o ../test/dot.png

firefox ../test/dot.png &
