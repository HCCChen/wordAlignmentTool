#!/bin/bash

printf "\E[1;34;40m\tDivide Law \E[0m\n"
./divideLaw.out
printf "\E[1;34;40m\tDivide Word \E[0m\n"
cd ICTCLAS_API
./dividWordByICTCLAS.o
cd ../
printf "\E[1;34;40m\tPreprocess for biligual context \E[0m\n"
./removeKnownAlign.out
./chPreProcess.out
./enPreProcess.out
cd ../data/
cp chBaseTrain enBaseTrain ../giza++ 
