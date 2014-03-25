/*
	Author: Paul Chen
	Date: 2014/02/27
	Target: 進入GIZA++之前的前處理
*/
#include "library.h"
using namespace std;

int explode(char divideChar, string originalString, string* stringAry);

int main(int argc, char* argv[]){
	const string CH_WORD_LIB_PATH = "../data/knownChWord";
	const string CH_LAW_PATH = "../data/chBaseTmp";
	const string CH_FIX_LAW_PATH = "../data/chBaseTrain";
	map<string, int> basicWordLib;
	fstream fin, fout;
	char buf[4096];
	string tmpStr, mergeWord, wordPool[512], lawSentence, emptyStr="";
	string punctuation[14] = {"，","。","、","；","：","＂","｛","｝","「","」","『","』","（","）"};
	int wordPoolSize, i,j,k;
	unsigned int loopCount, flag;

	//Load Word Lib
	fin.open(CH_WORD_LIB_PATH.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		basicWordLib[tmpStr] = 1;
	}
	fin.close();
	
	//Load File And Merge it!
	fin.open(CH_LAW_PATH.c_str(), ios::in);
	fout.open(CH_FIX_LAW_PATH.c_str(), ios::out);
	while(!fin.eof()){//For each Line
			fin.getline(buf, 4096);
			lawSentence.assign(buf);
			//filter punctuation
			for(i = 0; i < 14; i++){
				if(lawSentence.find(punctuation[i]) != string::npos){
					lawSentence = strReplaceAll(lawSentence, punctuation[i], emptyStr);
				}
			}
			//Merge Sentence
			flag = 1;//Phrase merge flag
			while(flag == 1){
				flag = 0;
				wordPoolSize = explode(' ', lawSentence, wordPool);
				lawSentence = "";
				for(loopCount = 0; loopCount < wordPoolSize-1; loopCount++){//for each word seg
					if(wordPool[loopCount+1].length() < 1){break;}
					tmpStr = wordPool[loopCount] + wordPool[loopCount+1];
					if(basicWordLib[tmpStr] != 1){//Not Find it!!
						tmpStr = wordPool[loopCount] + ' ';
					}
					else{//Find it!
						tmpStr = wordPool[loopCount] + wordPool[loopCount+1] + ' ';
						loopCount++;
						flag = 1;
					}
					lawSentence += tmpStr;
				}
				lawSentence += wordPool[loopCount];
			}
			fout << lawSentence << endl;
	}
	fin.close();
	fout.close();
	return 0;
}

int explode(char divideChar, string originalString, string* stringAry){
	int flag = 0, preFlag = 0 ,index = 0;
	int length = originalString.length();
	while(flag = originalString.find(divideChar, flag)){
		stringAry[index] = originalString.substr(preFlag, flag-preFlag);
		index++;
		if(flag > length || flag < 0){return index;}
		flag++;
		preFlag = flag;
	}
	return -1;
}

