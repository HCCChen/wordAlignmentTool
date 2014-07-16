/*
	Author: Paul Chen
	Date: 2014/07/16
	Target: 計算不同方法被EM analysis選到的比例
*/
#include "library.h"
using namespace std;

int main(int argc, char* argv[]){
	const string BASE_LIB_PATH = "../data/knownChWordForGetAlign";
	const string COMPAIR_FILE_PATH1 = "../data/cnadidateByCC";
	const string COMPAIR_FILE_PATH2 = "../data/cnadidateByDC";
	const string COMPAIR_FILE_PATH3 = "../data/cnadidateByFC";
	const string COMPAIR_FILE_PATH4 = "../data/cnadidateByLR";
	const string COMPAIR_FILE_PATH5 = "../data/cnadidateByMU";
	map<string, map<string, int>> basicLib;
	vector<string> segOfInput;
	char buf[4096];
	double i,basePair , totalPair, matchPair;
	string tmpStr;
	fstream fin, fout;
	//Load Basic File
	basePair = 0;
	fin.open(BASE_LIB_PATH.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.length() < 2){break;}
		segOfInput.clear();
		explode(",", tmpStr ,segOfInput);
		//Put in basicLib
		for(i = 1; i < segOfInput.size(); i++){
			if(segOfInput[i].length() < 1){break;}
			basePair++;
			basicLib[segOfInput[0]][segOfInput[i]] = 1;
		}
	}
	fin.close();

	//Compair Begin
	fin.open(COMPAIR_FILE_PATH5.c_str(), ios::in);
	totalPair = 0;
	matchPair = 0;
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.length() < 2){break;}
		segOfInput.clear();
		explode(",", tmpStr ,segOfInput);
		tmpStr = segOfInput[0];
		//Check Match percentage
		for(i = 1; i < segOfInput.size(); i++){
			if(segOfInput[i].length() < 1){break;}
			totalPair++;
			if(basicLib[tmpStr][segOfInput[i]] == 1){
				matchPair++;
				basicLib[tmpStr][segOfInput[i]] = -1;
				cout << tmpStr << "," << segOfInput[i] << endl;
			}
		}
	}
	fin.close();

	cout << "base pair: " << basePair << endl;
	cout << "total pair: " << totalPair << endl;
	cout << "match pair: " << matchPair << endl;
	cout << "match percentage: " << matchPair/basePair << endl;
	return 0;
}
