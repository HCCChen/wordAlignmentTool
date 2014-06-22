/*
	Author: Paul Chen
	Date: 2014/06/12
	Target: Random select candidate to Mutual check result
*/
#include "library.h"
using namespace std;

int main(int argc, char* argv[]){
	//const string INPUT_PATH = "../data/supportAlign";
	const string INPUT_PATH = "../data/evaluteResult";
	const string OUTPUT_PATH = "../data/randomSelect";
	const int MAX_SELECT = 500;
	char buf[4096];
	string tmpStr, chWord;
	fstream fout;
	int i, randNumber, selectNumber;
	vector<string> candidatePool;
	srand (time(NULL));

	loadFile(INPUT_PATH, candidatePool);

	fout.open(OUTPUT_PATH.c_str(), ios::out);
	for(i = 0; i < MAX_SELECT; i++)	{
		randNumber = rand() % candidatePool.size();
		tmpStr = candidatePool[randNumber];
		chWord = tmpStr.substr(0, tmpStr.find(","));
		if(chWord.length() < 6){
			i--;
			continue;
		}
		if(candidatePool[randNumber] != "EMPTY"){
			fout << candidatePool[randNumber] << endl;
			candidatePool[randNumber] = "EMPTY";
		}
		else{
			i--;
		}
	}
	fout.close();
	return 0;
}
