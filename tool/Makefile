all:
	make phraseMerge
	make evaluation
	make gizaDecoder
	make enPreProcess
	make chPreProcess
	make divideLaw
phraseMerge:
	g++ phraseMerge.cpp -std=c++0x -O2 -o phraseMerge.out
evaluation:
	g++ evaluation.cpp -std=c++0x -O2 -o evaluation.out
gizaDecoder:
	g++ gizaDecoder.cpp -std=c++0x -O2 -o gizaDecoder.out
enPreProcess:
	g++ enPreProcess.cpp -std=c++0x -O2 -o enPreProcess.out
chPreProcess:
	g++ chPreProcess.cpp -std=c++0x -O2 -o chPreProcess.out
divideLaw:
	g++ divideLaw.cpp -std=c++0x -O2 -o divideLaw.out
clear:
	rm *.out
