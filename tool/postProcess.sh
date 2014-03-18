#!/bin/bash

printf "\E[1;34;40m\tDecode!! \E[0m\n"
./gizaDecoder.out
./gizaA3Decoder.out
./phraseMerge.out
printf "\E[1;34;40m\tEvaluate Result \E[0m\n"
cat ../data/gizaDecodeResult ../data/gizaA3DecodeResult ../data/phraseResult > ../data/alignResult
./evaluation.out ../data/alignResult ../data/evaluteResult
