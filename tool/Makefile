CC = g++
CFLAGS = library.o -O2 -std=c++0x

all: library
	${CC} phraseMerge.cpp ${CFLAGS} -o phraseMerge.out
	${CC} evaluation.cpp ${CFLAGS} -o evaluation.out
	${CC} gizaDecoder.cpp ${CFLAGS} -o gizaDecoder.out
	${CC} gizaA3Decoder.cpp ${CFLAGS} -o gizaA3Decoder.out
	${CC} enPreProcess.cpp ${CFLAGS} -o enPreProcess.out
	${CC} chPreProcess.cpp ${CFLAGS} -o chPreProcess.out
	${CC} divideLaw.cpp ${CFLAGS} -o divideLaw.out
library: library.cpp
	${CC} library.h library.cpp ${CFLAGS} -c
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
clear:
	rm *.out
