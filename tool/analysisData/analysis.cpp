/*
	Author: Paul Chen
	Date: 2014//
	Target: 
*/
#include "../library.h"
using namespace std;

int main(int argc, char* argv[]){
	const string KNOWN_ALIGN_PATH = "../../data/knownWordAlign";
	const string CH_LAW_PATH = "../../data/chBaseTrain";
	const string EN_LAW_PATH = "../../data/enBaseTrain";
	char buf[4096];
	int pos1, pos2, i, matchCount = 0;
	string tmpStr, chLaw, enLaw;
	vector<string> chLib;
	vector<string> enLib;
	int *libFlag;
	fstream fin, fin2, fout;
	//Load KnownAlign File
	fin.open(KNOWN_ALIGN_PATH.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		pos1 = tmpStr.find(",");
		chLib.push_back(tmpStr.substr(0, pos1));
		enLib.push_back(tmpStr.substr(pos1 + 1));
	}
	fin.close();
	libFlag = new int[chLib.size()];
	for(i = 0; i < chLib.size(); i++){libFlag[i] = 0;}
	//Load BiLaw
	fin.open(CH_LAW_PATH.c_str(), ios::in);
	fin2.open(EN_LAW_PATH.c_str(), ios::in);
	while(!fin.eof() && !fin2.eof()){//For each law pair
		fin.getline(buf, 4096);
		chLaw.assign(buf);
		fin2.getline(buf, 4096);
		enLaw.assign(buf);
		//Check is Exist or not
		for(i = 0; i < chLib.size(); i++){
			if(chLaw.find(chLib[i]) != string::npos && enLaw.find(enLib[i]) != string::npos && libFlag[i] != 1){
				matchCount++;
				libFlag[i] = 1;
			}
		}
	}
	fin.close();
	fin2.close();
	cout << matchCount << endl;

	return 0;
}
