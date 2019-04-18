#!/bin/bash

make
# ./transform $1
./transform testFile1 > testFile1_gen
./transform testFile2 > testFile2_gen
./transform testFile3 > testFile3_gen
./transform testFile4 > testFile4_gen
make clean > /dev/null