/*
	Author: Paul Chen
	Date: 2014/2/28
	Target: Remove punctuation and sentence to lower
*/
#include "library.h"
using namespace std;

int main(int argc, char* argv[]){
	const string EN_LAW_PATH = "../data/enBaseTmp";
	const string EN_FIX_LAW_PATH = "../data/enBaseTrain";
	fstream fin, fout;
	char buf[4096];
	string lawSentence, emptyStr=" ";
	unsigned int loopCount;
	string punctuation[14] = {",",".",";",":","(",")","\"","\'", "\r"};
	int i;

	fin.open(EN_LAW_PATH.c_str(), ios::in);
	fout.open(EN_FIX_LAW_PATH.c_str(), ios::out);
	while(!fin.eof()){//For each Line
			fin.getline(buf, 4096);
			lawSentence.assign(buf);
			if(lawSentence.length() < 2){break;}
			lawSentence = strToLower(lawSentence);
			for(i = 0; i < 9; i++){
				if(lawSentence.find(punctuation[i]) != string::npos){
					lawSentence = strReplaceAll(lawSentence, punctuation[i], emptyStr);
				}
			}
			fout << lawSentence << endl;
	}
	return 0;
}
