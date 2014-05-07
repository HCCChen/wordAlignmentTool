/*
	Author: Paul Chen
	Date: 2014/03/19
	Target: Divide word and get POS tag by ICTCLAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include "ICTCLAS50.h"
#include "../library.h"

using namespace std;

string divideSentence(string sentence);
string removeTag(string sentence);

int main(int argc, char* argv[])
{
	const string INPUT_PATH = "../../data/chBase";
	const string OUTPUT_PATH = "../../data/chBaseTmp";
	const string TEST_OUTPUT_PATH = "testResult";
	vector<string> lawSeg;
	fstream fin, fout;
	char buf[4096];
	string tmpStr;
	map<string, int> wordSegLib;
	map<string, int>::iterator it;
	int i,j,k, pos1, pos2;

	if(!ICTCLAS_Init())			  
	{	printf("Init fails\n");
		return -1;	}

	unsigned int nItems=ICTCLAS_ImportUserDictFile("Data/userdict.txt",CODE_TYPE_UTF8);
	ICTCLAS_SaveTheUsrDic();
	cout << nItems << " user-defined lexical entries added!" << endl;
	//Load File and Divide it
	fin.open(INPUT_PATH.c_str(), ios::in);
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		tmpStr = divideSentence(buf);
		tmpStr = removeTag(tmpStr);
		fout << tmpStr << endl;
	}
	fin.close();
	fout.close();

	ICTCLAS_Exit();
	return 0;
}

string divideSentence(string sentence){
	int nPaLen=sentence.length();
	char* sRst=0;
	int nRstLen=0;
	string result;
	sRst=(char *)malloc(nPaLen*6);
	nRstLen=ICTCLAS_ParagraphProcess(sentence.c_str(),nPaLen,sRst ,CODE_TYPE_UTF8,1);
	result.assign(sRst);
	free(sRst);
	return result;
}


string removeTag(string sentence){
	int i, pos;
	string tmpStr, finalResult = "";
	vector<string> wordSeg;
	explode(' ', sentence, wordSeg);
	for(i = 0; i < wordSeg.size(); i++){
		tmpStr = wordSeg[i].substr(0, wordSeg[i].find('/'));
		finalResult += tmpStr + " ";
	}
	return finalResult;
}
