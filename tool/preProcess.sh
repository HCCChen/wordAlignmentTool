#!/bin/bash

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
#chBaseTmp2 -> chBaseTmp
#enBaseTmp -> enBase
cd ../data
mv chBaseTmp2 chBaseTmp
mv enBaseTmp enBase
cd ../tool

printf "\E[1;34;40m\tRevert English word to basic type \E[0m\n"
#chBaseTmp -> chBaseTmp
#enBase -> enBaseTmp
cd englishLemma/
./getLemma.sh ../../data/enBase ../../data/
cd ../
./revertEnglishWord.out

printf "\E[1;34;40m\tRemove known align! \E[0m\n"
#chBaseTmp -> chBaseTmp
#enBaseTmp -> enBase
cd ../data
mv enBaseTmp enBase
cd ../tool
#chBaseTmp -> chBaseTmp2
#enBase -> enBaseTmp
./removeKnownAlign.out
#chBaseTmp2 -> chBaseTmp
#enBaseTmp -> enBaseTmp
cd ../data
mv chBaseTmp2 chBaseTmp
cd ../tool

printf "\E[1;34;40m\tOther pre-process and move them to 'giza++' dictionary! \E[0m\n"
#chBaseTmp2 -> chBaseTrain
#enBaseTmp -> enBaseTrain
./chPreProcess.out
./enPreProcess.out
cd ../data
cp chBaseTrain enBaseTrain ../giza++/
cd ../tool
