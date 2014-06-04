CC = g++
CFLAGS = library.o -std=c++11 -pthread -I./inlcude -L./lib -lboost_thread -O2

all: library
	${CC} analysis.cpp ${CFLAGS} -o analysis.out
library: ../library.cpp
	${CC} ../library.h ../library.cpp -O2 -std=c++11 -c
analysis: analisys.cpp
	${CC} analysis.cpp ${CFLAGS} -o analysis.out
getSourceList: getSourceList.cpp
	${CC} getSourceList.cpp ${CFLAGS} -o getSourceList.out
	
clear:
	rm *.out
