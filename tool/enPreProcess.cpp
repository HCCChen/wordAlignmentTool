/*
	Author: Paul Chen
	Date: 2014/2/28
	Target: 
*/
#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]){
	const string EN_LAW_PATH = "../data/enBase";
	const string EN_FIX_LAW_PATH = "../data/enBaseTrain";
	fstream fin, fout;
	char buf[4096];
	string lawSentence;
	unsigned int loopCount;

	fin.open(EN_LAW_PATH.c_str(), ios::in);
	fout.open(EN_FIX_LAW_PATH.c_str(), ios::out);
	while(!fin.eof()){//For each Line
			fin.getline(buf, 4096);
			for(loopCount = 0; loopCount < strlen(buf); loopCount++){
				if(buf[loopCount] >= 'A' && buf[loopCount] <= 'Z'){//To lower
					buf[loopCount] += 'a' - 'A';
				}
			}
			lawSentence.assign(buf);
			fout << lawSentence << endl;
	}

	return 0;
}
