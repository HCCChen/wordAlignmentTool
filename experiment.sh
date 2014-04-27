#!/bin/bash

printf "\E[1;32;40mBegin Of Experiment! \E[0m\n"
printf "\E[0;33;40mPre-Processing of Word Alignment! \E[0m\n"
cd tool 
./preProcess.sh
printf "\E[0;33;40mAlign Word By GIZA++! \E[0m\n"
cd ../giza++
./alignWord.sh
printf "\E[0;33;40mPost-Processing of Word Alignment! \E[0m\n"
cd ../tool
./postProcess.sh
printf "\E[1;32;40mEnd Of Experiment! \E[0m\n"
