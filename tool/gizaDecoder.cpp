/*
	Author: Paul Chen
	Date: 2014/3/1
	Target: Decode Giza++ t3 result, index start from 2
*/
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]){
	const string SOURCE_LANGUAGE_PATH = "../giza++/chBaseTrain.vcb";
	const string TARGET_LANGUAGE_PATH = "../giza++/enBaseTrain.vcb";
	const string GIZA_RESULT_PATH = "../giza++/c2e.t3.final";
	const string OUTPUT_PATH = "../data/gizaDecodeResult";
	fstream fin, fout;
	string chLib[50000], enLib[50000];
	int pos1, pos2, loopIdx;
	int sourceIdx, targetIdx;
	char buf[4096];
	string strBuf, subStrBuf, alignScore;

	//Load Word-Index File
	loopIdx = 2;
	fin.open(SOURCE_LANGUAGE_PATH, ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		strBuf.assign(buf);
		pos1 = strBuf.find(' ');
		pos2 = strBuf.find_last_of(' ');
		chLib[loopIdx++] = strBuf.substr(pos1 + 1, pos2 - pos1-1);
	}
	fin.close();

	loopIdx = 2;
	fin.open(TARGET_LANGUAGE_PATH, ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		strBuf.assign(buf);
		pos1 = strBuf.find(' ');
		pos2 = strBuf.find_last_of(' ');
		enLib[loopIdx++] = strBuf.substr(pos1 + 1, pos2 - pos1-1);
	}
	fin.close();

	//Decode "c2e.t3.final"

	loopIdx = 2;
	fin.open(GIZA_RESULT_PATH, ios::in);
	fout.open(OUTPUT_PATH, ios::out);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		strBuf.assign(buf);
		pos1 = strBuf.find(' ');
		pos2 = strBuf.find_last_of(' ');
		//Divide Data
		subStrBuf = strBuf.substr(0, pos1);
		sourceIdx = atoi(subStrBuf.c_str());
		if(sourceIdx == 0)	{continue;}
	
		subStrBuf = strBuf.substr(pos1 + 1, pos2 - pos1);
		targetIdx = atoi(subStrBuf.c_str());

		alignScore = strBuf.substr(pos2 + 1);
	
		//Filter and output	
		if(alignScore.find('e') != string::npos){continue;}	//Probability is too low
		if(chLib[sourceIdx].length() < 6){continue;}
		fout << chLib[sourceIdx] << "," << enLib[targetIdx] << "," << alignScore << endl;
	}
	
	
	fin.close();
	fout.close();

	return 0;
}

