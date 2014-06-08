#!/bin/bash

$switchRevert = "false"

cd ../data
rm chBase* enBase*
cd ../tool

printf "\E[1;34;40m\tDivide Law \E[0m\n"
#comtext -> chBase/enBase
./divideLaw.out
printf "\E[1;34;40m\tDivide Word \E[0m\n"
#chBase -> chBaseTmp
#enBase -> enBase
cd ICTCLAS_API
./dividWordByICTCLAS.o
cd ../

printf "\E[1;34;40m\tRemove known align! \E[0m\n"
#chBaseTmp -> chBaseTmp2
#enBase -> enBaseTmp
./removeKnownAlign.out

printf "\E[1;34;40m\tGenerate word pair by n-gram \E[0m\n"
#chBase + enBaseTmp -> ngramPairResult
./getAlignPair.out > ../data/ngramPairResult
./evaluation.out ../data/ngramPairResult ../data/evaluteResult
