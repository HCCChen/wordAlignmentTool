CC = g++
CFLAGS = library.o -std=c++11 -pthread -I./inlcude -L./lib -lboost_thread -O2

all: library
	${CC} phraseMerge.cpp ${CFLAGS} -o phraseMerge.out
	${CC} evaluation.cpp ${CFLAGS} -o evaluation.out
	${CC} gizaDecoder.cpp ${CFLAGS} -o gizaDecoder.out
	${CC} gizaA3Decoder.cpp ${CFLAGS} -o gizaA3Decoder.out
	${CC} enPreProcess.cpp ${CFLAGS} -o enPreProcess.out
	${CC} chPreProcess.cpp ${CFLAGS} -o chPreProcess.out
	${CC} divideLaw.cpp ${CFLAGS} -o divideLaw.out
	${CC} alignmentFilter.cpp ${CFLAGS} -o alignmentFilter.out
	${CC} removeKnownAlign.cpp ${CFLAGS} -o removeKnownAlign.out
	${CC} revertEnglishWord.cpp ${CFLAGS} -o revertEnglishWord.out
	${CC} filtKnownAlignLib.cpp ${CFLAGS} -o filtKnownAlignLib.out
	${CC} getAlignPair.cpp ${CFLAGS} -o getAlignPair.out
	${CC} supportModel.cpp ${CFLAGS} -o supportModel.out
	${CC} randomSelect.cpp ${CFLAGS} -o randomSelect.out
	${CC} compairMatchAmount.cpp ${CFLAGS} -o compairMatchAmount.out
library: library.cpp
	${CC} library.h library.cpp -O2 -std=c++11 -c
phraseMerge: phraseMerge.cpp
	${CC} phraseMerge.cpp ${CFLAGS} -o phraseMerge.out
evaluation: evaluation.cpp
	${CC} evaluation.cpp ${CFLAGS} -o evaluation.out
gizaDecoder: gizaDecoder.cpp
	${CC} gizaDecoder.cpp ${CFLAGS} -o gizaDecoder.out
gizaA3Decoder: gizaA3Decoder.cpp
	${CC} gizaA3Decoder.cpp ${CFLAGS} -o gizaA3Decoder.out
enPreProcess: enPreProcess.cpp
	${CC} enPreProcess.cpp ${CFLAGS} -o enPreProcess.out
chPreProcess: chPreProcess.cpp
	${CC} chPreProcess.cpp ${CFLAGS} -o chPreProcess.out
divideLaw: divideLaw.cpp
	${CC} divideLaw.cpp ${CFLAGS} -o divideLaw.out
alignmentFilter: alignmentFilter.cpp
	${CC} alignmentFilter.cpp ${CFLAGS} -o alignmentFilter.out
removeKnownAlign: removeKnownAlign.cpp
	${CC} removeKnownAlign.cpp ${CFLAGS} -o removeKnownAlign.out
revertEnglishWord: revertEnglishWord.cpp
	${CC} revertEnglishWord.cpp ${CFLAGS} -o revertEnglishWord.out
filtKnownAlignLib: filtKnownAlignLib.cpp
	${CC} filtKnownAlignLib.cpp ${CFLAGS} -o filtKnownAlignLib.out
getAlignPair: getAlignPair.cpp 
	${CC} getAlignPair.cpp ${CFLAGS} -o getAlignPair.out
supportModel: supportModel.cpp
	${CC} supportModel.cpp ${CFLAGS} -o supportModel.out
randomSelect: randomSelect.cpp
	${CC} randomSelect.cpp ${CFLAGS} -o randomSelect.out
compairMatchAmount: compairMatchAmount.cpp
	${CC} compairMatchAmount.cpp ${CFLAGS} -o compairMatchAmount.out
	


clear:
	rm *.out
