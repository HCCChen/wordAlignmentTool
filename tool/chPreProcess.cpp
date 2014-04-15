/*
	Author: Paul Chen
	Date: 2014/02/27
	Target: 進入GIZA++之前的前處理
*/
#include "library.h"
using namespace std;

int explode(char divideChar, string originalString, string* stringAry);
/*
	return 0: no match
	return 1: all match
	return 2: part match
*/
int checkLib(string inputStr, vector<string> &basicLib);

int main(int argc, char* argv[]){
	const string CH_WORD_LIB_PATH = "../data/knownChWord";
	const string CH_LAW_PATH = "../data/chBaseTmp";
	const string CH_FIX_LAW_PATH = "../data/chBaseTrain";
//	map<string, int> basicWordLib;//<known string, using time>
	vector<string> basicWordLib;
	vector<int> basicWordFlag;
	fstream fin, fout;
	char buf[4096];
	string tmpStr, mergeWord, wordPool[512], lawSentence, emptyStr="";
	string punctuation[16] = {" ，"," 。"," 、"," ；"," ："," ＂"," ｛"," ｝"," 「"," 」"," 『"," 』"," （"," ）"," 　", "\r"};
	int wordPoolSize, i,j,k, flag, inLineFlag;

	//Load Word Lib
	fin.open(CH_WORD_LIB_PATH.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
	//	basicWordLib[tmpStr] = 0;
		basicWordLib.push_back(tmpStr);
		basicWordFlag.push_back(0);
	}
	fin.close();
	
	//Load File And Merge it!
	fin.open(CH_LAW_PATH.c_str(), ios::in);
	fout.open(CH_FIX_LAW_PATH.c_str(), ios::out);
	while(!fin.eof()){//For each Line
			fin.getline(buf, 4096);
			lawSentence.assign(buf);
			//filter punctuation
			for(i = 0; i < 16; i++){
				if(lawSentence.find(punctuation[i]) != string::npos){
					lawSentence = strReplaceAll(lawSentence, punctuation[i], emptyStr);
				}
			}
			//Merge Sentence
			wordPoolSize = explode(' ', lawSentence, wordPool);
			for(i = 0, lawSentence = "",tmpStr = "", inLineFlag = -1; i < wordPoolSize; i++){//for each word seg
				tmpStr += wordPool[i];
				//Check known word lib
				flag = checkLib(tmpStr, basicWordLib);
				switch(flag){
					case 0://No match->output
						if(inLineFlag == -1){//Without Buffer->output
							lawSentence += tmpStr + ' ';
							tmpStr = "";
							inLineFlag = -1;				
						}
						else{//Have Buffer-> back to first seg
							i = inLineFlag;
							lawSentence += wordPool[i] + ' ';
							tmpStr = "";
							inLineFlag = -1;				
						}
					break;
					case 1://All match->Merge
						lawSentence += tmpStr + ' ';
						tmpStr = "";
						inLineFlag = -1;
					break;
					case 2://Part match->continue check
						if(inLineFlag == -1){
							inLineFlag = i;//Have Buffer
						}
						continue;
					break;
				}
			}
			if(inLineFlag != -1){//Have buffer -> Clear the buffer
				for(i = inLineFlag; i < wordPoolSize; i++){
					lawSentence += wordPool[i] + ' ';
				}
			}
			//Output to file
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

int checkLib(string inputStr, vector<string> &basicLib){
	int i, flag;
	//Default: no match
	for(i = 0, flag = 0; i < basicLib.size(); i++){
		//All match
		if(inputStr == basicLib[i]){
			flag = 1;
			break;
		}
		//Part match
		else if(flag == 0 && basicLib[i].find(inputStr) != string::npos){
			flag = 2;
		}
	}
	return flag;
}
