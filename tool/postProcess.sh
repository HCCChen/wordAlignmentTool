#!/bin/bash

printf "\E[1;34;40m\tDecode!! \E[0m\n"
./gizaDecoder.out
./phraseMerge.out
printf "\E[1;34;40m\tEvaluate Result \E[0m\n"
./evaluation.out ../data/gizaDecodeResult ../data/evaluteResultBasic
./evaluation.out ../data/phraseResult ../data/evaluteResultPhrase
