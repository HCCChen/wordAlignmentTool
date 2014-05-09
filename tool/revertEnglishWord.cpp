/*
	Author: Paul Chen
	Date: 2014/05/09
	Target: Revert english word to original type by stanford pos-tagger
*/
#include "library.h"
using namespace std;

int main(int argc, char* argv[]){
	const string ENLIB_PATH = "../data/enBaseTmp.xml";
	const string CONTEXT_PATH = "../data/enBaseTmp";
	const string OUTPUT_PATH = "../data/enBaseTmp2";
	char buf[4096];
	string tmpStr, baseWord, lemmaWord, dividFlag = " ";
	map<string, string> lemmaPair;
	vector<string> wordSeg;
	fstream fin, fout;
	int i,j,pos1,pos2;
	fin.open(ENLIB_PATH.c_str(), ios::in);
	while(!fin.eof()){//Load lemma Lib
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.find("<word>") != string::npos){
			pos1 = tmpStr.find(">");
			pos2 = tmpStr.find_last_of("<");
			baseWord = tmpStr.substr(pos1+1, pos2 - pos1-1);
		}
		else if(tmpStr.find("<lemma>") != string::npos){
			pos1 = tmpStr.find(">");
			pos2 = tmpStr.find_last_of("<");
			lemmaWord = tmpStr.substr(pos1+1, pos2 - pos1-1);
			if(baseWord != lemmaWord && lemmaPair.find(baseWord) == lemmaPair.end()){
				lemmaPair[baseWord] = lemmaWord;
			}
		}
	}
	fin.close();
	//Load context and revert english word
	fin.open(CONTEXT_PATH.c_str(), ios::in);
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	
	while(!fin.eof()){//Load Context
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.length() < 2){break;}
		tmpStr = strToLower(tmpStr);
		explode(" ", tmpStr, wordSeg);
		for(i = 0; i < wordSeg.size(); i++){
			if(lemmaPair.find(wordSeg[i]) != lemmaPair.end()){//Find original type
				wordSeg[i] = lemmaPair[wordSeg[i]];
			}
		}
		fout << mergeSequence(dividFlag, wordSeg) << endl;
	}
	fin.close();
	fout.close();
	
	return 0;
}
