/*
	Author: Paul Chen
	Date: 2014/02/27
	Target: 進入GIZA++之前的前處理
*/
#include "library.h"
using namespace std;

int explode(char divideChar, string originalString, string* stringAry);

int main(int argc, char* argv[]){
	const string CH_LAW_PATH = "../data/chBaseTmp";
	const string CH_FIX_LAW_PATH = "../data/chBaseTrain";
//	map<string, int> basicWordLib;//<known string, using time>
	vector<string> basicWordLib;
	vector<int> basicWordFlag;
	fstream fin, fout;
	char buf[4096];
	string tmpStr, mergeWord, wordPool[512], lawSentence, emptyStr="";
	string punctuation[16] = {" ，"," 。"," 、"," ；"," ："," ＂"," ｛"," ｝"," 「"," 」"," 『"," 』","（"," ）"," 　", "\r"};
	int wordPoolSize, i,j,k, flag, inLineFlag, checkLongerFlag;

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
//			fout << lawSentence << endl;
			for(i = wordPoolSize-1, lawSentence = "",tmpStr = ""; i >= 0; i--){//for each word seg
				tmpStr = wordPool[i];
				//Check known word lib
				if(tmpStr == "條"){
					if(i-1 >= 0 && wordPool[i-1].find("第") != string::npos){
						lawSentence = wordPool[i-1] + tmpStr + ' ' + lawSentence;
						i--;
					}
					else if(i-2 >= 0 && wordPool[i-2].find("第") != string::npos){
						lawSentence = wordPool[i-2] + wordPool[i-1] + tmpStr + ' ' + lawSentence;
						i = i-2;
					}
				}
				else if(tmpStr == "元" || tmpStr == "歲" || tmpStr == "項" || tmpStr == "款" || tmpStr == "小時" || tmpStr == "編" || tmpStr == "章"){
					lawSentence = wordPool[i-1] + tmpStr + ' ' + lawSentence;
					i--;
				}
				else{
					lawSentence = tmpStr + ' ' + lawSentence;
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
