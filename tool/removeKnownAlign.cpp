/*
	Author: Paul Chen
	Date: 2014/05/07
	Target: Remove known align word
*/
#include "library.h"
using namespace std;

string removeInlineSubStr(string origin, string substr);
string mergeSequenceVector(vector<string> wordSeq);

int main(int argc, char* argv[]){
	const string ALIGN_LIB_PATH = "../data/languageBase/cedictLib";
	const string ALIGN_LIB_PATH2 = "../data/languageBase/knownOtherAlignWord";
	const string CH_CONTEXT_PATH = "../data/chBaseTmp";
	const string EN_CONTEXT_PATH = "../data/enBase";
	const string CH_OUTPUT_PATH = "../data/chBaseTmp2";
	const string EN_OUTPUT_PATH = "../data/enBaseTmp";
	const string EMPTY_FLAG = "EMPTY";
	const bool ALIGN_LIB1_SWITCH = true; //True = use it!!
	const bool ALIGN_LIB2_SWITCH = true; //True = use it!!
	char buf[4096];
	vector<string> translateBuf;
	map<string, vector<string>> knownLib;
	string tmpStr, chWord, chSentence, enSentence;
	fstream fin, fin2, fout, fout2;
	int i, j, k, matchCount, matchFlag;

	if(ALIGN_LIB1_SWITCH == true){
			fin.open(ALIGN_LIB_PATH.c_str(), ios::in);
			while(!fin.eof()){//Load known align lib
					fin.getline(buf, 4096);
					tmpStr.assign(buf);
					chWord = tmpStr.substr(0, tmpStr.find(" "));
					tmpStr = tmpStr.substr(tmpStr.find("/")+1);
					explode('/', tmpStr, translateBuf);
					for(i = 0;i < translateBuf.size() -1; i++){
							knownLib[chWord].push_back(translateBuf[i]);
					}
			}
			fin.close();
	}
	if(ALIGN_LIB2_SWITCH == true){
			fin.open(ALIGN_LIB_PATH2.c_str(), ios::in);
			while(!fin.eof()){//Load known align lib
					fin.getline(buf, 4096);
					tmpStr.assign(buf);
					chWord = tmpStr.substr(0, tmpStr.find(","));
					tmpStr = tmpStr.substr(tmpStr.find(",")+1);
					knownLib[chWord].push_back(tmpStr);
			}
			fin.close();
	}

	//Load Context
	fin.open(CH_CONTEXT_PATH, ios::in);
	fin2.open(EN_CONTEXT_PATH, ios::in);
	fout.open(CH_OUTPUT_PATH, ios::out);
	fout2.open(EN_OUTPUT_PATH, ios::out);
	matchCount = 0;
	while(!fin.eof() && !fin2.eof()){//For each pair
		fin.getline(buf, 4096);
		chSentence.assign(buf);
		fin2.getline(buf, 4096);
		enSentence.assign(buf);
		if(chSentence.length() < 3 || enSentence.length() < 3){break;}
		explode(' ', chSentence, translateBuf);
		for(i = 0, matchFlag = 0;i < translateBuf.size() -1; i++, matchFlag = 0){//For each word segmentation
			if(knownLib.find(translateBuf[i]) != knownLib.end() && translateBuf[i].length() > 3){//ChWord Match
				for(j = 0 ;j < knownLib[translateBuf[i]].size(); j++){
					tmpStr = knownLib[translateBuf[i]][j] + " ";
					if(enSentence.find(tmpStr) != string::npos){//Pair match
						
						//cout << translateBuf[i] << " : " << knownLib[translateBuf[i]][j] << endl;
						//Remove Ch Word
						matchFlag = 1;
						//Remove En Word
						enSentence = removeInlineSubStr(enSentence, knownLib[translateBuf[i]][j]);
						matchCount++;
					}
				}
			}
			if(matchFlag == 1){
				translateBuf[i] = "EMPTY";
			}
		}
		//merge each pair
		fout << mergeSequenceVector(translateBuf) << endl;
		fout2 << enSentence << endl;
	}
	cout << matchCount << " pairs have be remove" << endl;
	fin.close();
	fin2.close();
	fout.close();
	fout2.close();
	return 0;
}


string removeInlineSubStr(string origin, string substr){
	int i;
	size_t pos = origin.find(substr);
	size_t headPos, tailPos;
	string headStr, tailStr, result;
	headPos = origin.find_last_of(" ", pos);
	tailPos = origin.find(" ", pos + substr.length() - 1);

	if(pos == 0 && origin.length() > substr.length()){//substr appear in the head of origin
		result = origin.substr(substr.length()+1);
	}
	else if(headPos != string::npos){
		headStr = origin.substr(0, headPos);	
		if(tailPos == string::npos)	{
			tailStr = "";
		}
		else{
			tailStr = origin.substr(tailPos);
		}	
		result = headStr + tailStr;
	}
	else{
		result = origin;
	}
	return result;
}

string mergeSequenceVector(vector<string> wordSeq){
	int i;
	string result = "";
	for(i = 0; i < wordSeq.size()-1 ;i++){
		if(wordSeq[i] == "EMPTY"){continue;}
		result += wordSeq[i] + " ";
	}
	return result;
}
