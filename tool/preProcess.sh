#!/bin/bash

printf "\E[1;34;40m\tDivide Law \E[0m\n"
./divideLaw.out
printf "\E[1;34;40m\tDivide Word \E[0m\n"
rm ../data/chBaseTmp
/usr/local/scws/bin/scws -c utf8 -r /usr/local/scws/etc/rules_cht.utf8.ini -d /usr/local/scws/etc/dict_cht.utf8.xdb -i /home/hsng/wordAlignment/data/chBase -o /home/hsng/wordAlignment/data/chBaseTmp
printf "\E[1;34;40m\tFlit Word \E[0m\n"
./chPreProcess.out
./enPreProcess.out
cd ../data/
cp chBaseTrain enBaseTrain ../giza++ 
