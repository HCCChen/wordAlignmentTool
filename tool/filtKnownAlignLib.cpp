/*
	Author: Paul Chen
	Date: 2014/05/10
	Target: Remove domain pair in CEDICT
*/
#include "library.h"
using namespace std;

int main(int argc, char* argv[]){
	const string CEDICT_PATH = "../data/languageBase/cedict_ts.u8";
	const string KNOWN_LIB_PATH = "../data/knownWordAlign";
	const string OUTPUT_PATH = "../data/languageBase/cedictLib";
	char buf[4096];
	string tmpStr, word;
	map<string, int> knownLib;
	fstream fin, fout;
	fin.open(KNOWN_LIB_PATH.c_str(), ios::in);
	while(!fin.eof()){//Load known align lib
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		word = tmpStr.substr(0, tmpStr.find(","));
		if(knownLib.find(word) == knownLib.end()){
			knownLib[word] = 1;
		}
	}
	fin.close();

	fin.open(CEDICT_PATH.c_str(), ios::in);
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	while(!fin.eof()){//Load known align lib
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		word = tmpStr.substr(0, tmpStr.find(" "));
		if(knownLib.find(word) == knownLib.end()){
			fout << tmpStr << endl;
		}
	}
	fin.close();
	fout.close();
	return 0;
}
