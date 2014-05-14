/*
	Author: Paul Chen
	Date: 2014/03/18
	Target: Filter and merge the wrong align result
*/
#include "library.h"
using namespace std;

int getPairFrequency(string chWord, string enWord, vector<string> chContext, vector<string> enContext);

int main(int argc, char* argv[]){
	char buf[4096];
	//"alignScore" could be alignment probability(double) or frequency(int)
	string tmpStr, chWord, alignWord, alignScore;
	const string INPUT_PATH = "../data/candidatePair";
	const string OUTPUT_PATH = "../data/alignResult";
	const string CH_CONTEXT_PATH = "../data/chBase";
	const string EN_CONTEXT_PATH = "../data/enBase";
	const string SPECIAL_LIB_PATH = "../data/languageBase/specialWordLibEn";
	vector<string> filePath;
	vector<string> chContext;
	vector<string> enContext;
	map<string, int> filterLib;
	fstream fin, fout;
	map<string, int> alignResult;
	map<string, int>::iterator iter;
	int i, pos1, pos2, pairFrequency;

	regex_t regexWord, regexAlign;
	int reti, reti2;
	reti = regcomp(&regexWord, "[a-zA-Z0-9]", 0);
	reti2 = regcomp(&regexAlign, "[0-9(){},.;:]", 0);
	//Get file Path from parameter
	if(argc > 1){
		for(i = 1; i < argc; i++){
			tmpStr.assign(argv[i]);
			filePath.push_back(tmpStr);
		}
	}
	else{
		filePath.push_back(INPUT_PATH);
	}

	//Load information file
	loadFile(CH_CONTEXT_PATH, chContext);
	loadFile(EN_CONTEXT_PATH, enContext);
	loadFile(SPECIAL_LIB_PATH, filterLib);
	//Load Alignment Result and record it!
	for(i = 0; i < filePath.size(); i++){//For each file
		fin.open(filePath[i].c_str(), ios::in);
		while(!fin.eof()){//For each pair
			pairFrequency = 0;
			fin.getline(buf, 4096);
			tmpStr.assign(buf);
			pos1 = tmpStr.find(',');
			pos2 = tmpStr.find_last_of(',');
			//Divide Information
			chWord = tmpStr.substr(0, pos1);
			alignWord = tmpStr.substr(pos1+1, pos2 - pos1 - 1);
			alignScore = tmpStr.substr(pos2 + 1);
			//Filter
			reti = regexec(&regexWord, chWord.c_str(), 0, NULL, 0);
			reti2 = regexec(&regexAlign, alignWord.c_str(), 0, NULL, 0);
			if( !reti || !reti2 || chWord.length() < 3 || 
				alignWord.length() < 3 || alignScore.find('e') != string::npos||
				filterLib.find(alignWord) != filterLib.end()){
				continue;
			}
			else if(alignResult.find(tmpStr) == alignResult.end()){
				pairFrequency = getPairFrequency(chWord, alignWord, chContext, enContext);
				alignResult[tmpStr] = pairFrequency;
			}
		}
		fin.close();
	}
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	for(iter = alignResult.begin(); iter != alignResult.end(); iter++){
		fout << iter->first << "," << iter->second <<endl;
	}
	fout.close();

	return 0;
}


int getPairFrequency(string chWord, string enWord, vector<string> chContext, vector<string> enContext){
	int i, pairFreq;
	for(i = 0, pairFreq = 0; i < chContext.size(); i++){
		if(chContext[i].find(chWord) != string::npos && enContext[i].find(enWord) != string::npos){
			pairFreq++;
		}
	}
	return pairFreq;
}
