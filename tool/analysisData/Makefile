CC = g++
CFLAGS = library.o -O2 -std=c++0x

all: library
	${CC} analysis.cpp ${CFLAGS} -o analysis.out
library: ../library.cpp
	${CC} ../library.h ../library.cpp -O2 -std=c++0x -c
analysis: analisys.cpp
	${CC} analysis.cpp ${CFLAGS} -o analysis.out
	
clear:
	rm *.out
