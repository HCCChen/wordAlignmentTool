#!/bin/bash

echo 'Divide Law!'
./divideLaw.out
echo 'Divide Word!'
rm ../data/chBaseTmp
/usr/local/scws/bin/scws -c utf8 -r /usr/local/scws/etc/rules_cht.utf8.ini -d /usr/local/scws/etc/dict_cht.utf8.xdb -i /home/hsng/wordAlignment/data/chBase -o /home/hsng/wordAlignment/data/chBaseTmp
echo 'Filt Word!'
./chPreProcess.out
./enPreProcess.out
