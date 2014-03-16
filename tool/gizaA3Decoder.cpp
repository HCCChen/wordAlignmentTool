/*
	Author: Paul Chen
	Date: 2014/3/16
	Target: Decode Giza++ A3 result, index start from 2
*/
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <map>

using namespace std;

typedef struct alignLib{
	map<string, int>alignWordLib;	//For basic Align Word(Map)

	string mergeWord[128];	//For post-process align Word
	int mergeCount;

}ALIGNPOOL;

bool decodeA3Result(string chSentence, string enSentence, map<string, ALIGNPOOL> &alignResult);

int main(int argc, char* argv[]){
	const string GIZA_RESULT_PATH = "../wordAlignment/c2e.A3.final";
	const string OUTPUT_PATH = "../data/gizaA3DecodeResult";
	fstream fin, fout;
	map<string, ALIGNPOOL> alignResult;
	int i,j;//loop Counter
	int pos1, pos2, flag;
	string chSentence, enSentence;
	char buf[4096];

	//Decode "c2e.t3.final"

	fin.open(GIZA_RESULT_PATH, ios::in);
	fout.open(OUTPUT_PATH, ios::out);
	flag = 0;
	while(!fin.eof()){
		fin.getline(buf, 4096);
		switch(flag){
			case 0:
				flag++;
			break;
			case 1:
				enSentence.assign(buf);
				flag++;
			break;
			case 2:
				chSentence.assign(buf);
				flag = 0;
				decodeA3Result(chSentence, enSentence, alignResult);
				//Do Decoder
			break;
		}
	}
	fin.close();
	fout.close();

	return 0;
}

bool decodeA3Result(string chSentence, string enSentence, map<string, ALIGNPOOL> &alignResult){
	cout << chSentence << endl << enSentence << endl << "--------------" << endl;
	//Divide English Word
	//Divide Chinese Word and alignment number
	//Convert it !
}
