/*
	Author: Paul Chen
	Date: 2014/03/18
	Target: Filter and merge the wrong align result
*/
#include "library.h"
using namespace std;

int getPairFrequency(string chWord, string enWord, vector<string> &chContext, vector<string> &enContext, int &fCh, int &fEn, int &totalLine, double &fc);
bool generateCandidate(string chWord, string alignWord, string alignScore, map<string, string> &alignResult, vector<string> &chContext, vector<string> &enContext);
bool testFun(int idx, map<string, string> &alignResult ,vector<string> &chContext, vector<string> &enContext);

int main(int argc, char* argv[]){
	char buf[4096];
	//"alignScore" could be alignment probability(double) or frequency(int)
	string tmpStr, chWord, alignWord, alignScore;
	const string INPUT_PATH = "../data/candidatePair";
	const string OUTPUT_PATH = "../data/alignResult";
	const string CH_CONTEXT_PATH = "../data/chBase";
	const string EN_CONTEXT_PATH = "../data/enBase";
	const string SPECIAL_LIB_PATH = "../data/languageBase/specialWordLibEn";
	const int MAX_THREAD = 256;
	vector<string> filePath;
	vector<string> chContext;
	vector<string> enContext;
	map<string, int> filterLib;
	fstream fin, fout;
	map<string, string> alignResult;
	map<string, string>::iterator iter;
	int i,j, pos1, pos2, threadFlag, processCount = 0;
	std:thread 	t[256];

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
			processCount++;
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
				//Multi-thread for candidate
			//	generateCandidate(chWord, alignWord, alignScore, alignResult, chContext, enContext);
				t[threadFlag++] = std::thread(generateCandidate, chWord, alignWord, alignScore, std::ref(alignResult), std::ref(chContext), std::ref(enContext));
				if(threadFlag == MAX_THREAD){//Wait thread clear!
					cout << processCount << " thread finish! " << endl;
					for(j = 0; j < MAX_THREAD; j++){
						t[j].join();
					}
					threadFlag = 0;
				}
			}
		}
		for(j = 0; j < threadFlag; j++){
			t[j].join();
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

int getPairFrequency(string chWord, string enWord, vector<string> &chContext, vector<string> &enContext, int &fCh, int &fEn, int &totalLine, double &fc){
	int i,j,pairFreq;
	double enWordCount = 0 ,enSegCount = 0;
	for(j = 0; j < enWord.size(); j++){if(enWord[j] == ' '){enWordCount++;}}
	for(i = 0, pairFreq = 0, fCh = 0, fEn = 0, fc = 0, totalLine = chContext.size(); i < totalLine; i++){
		if(chContext[i].find(chWord) != string::npos && enContext[i].find(enWord) != string::npos){//Chinese and English appear at the same time
			pairFreq++;
			//get the number of english word seg
			for(j = 0; j < enContext[i].size(); j++){if(enContext[i][j] == ' '){enSegCount++;}}
			//Calculate "Fractional count"
			fc += 1/(enSegCount-enWordCount);
			continue;
		}
		if(chContext[i].find(chWord) != string::npos){fCh++;}
		if(enContext[i].find(enWord) != string::npos){fEn++;}
	}
	return pairFreq;
}

bool generateCandidate(string chWord, string alignWord, string alignScore, map<string, string> &alignResult, vector<string> &chContext, vector<string> &enContext){
	string tmpStr = chWord + "," + alignWord;
	string alignFreq = "0", alignProbability = "0";
	vector<string> scoreSeq;
	int pairFrequency, chFreq, enFreq, totalLine;
	double mu, cc, lr, dc, fc;
	//if Pair is exist: update!!
	if(alignResult.find(tmpStr) != alignResult.end()){
		explode(',', alignResult[tmpStr], scoreSeq);
		alignFreq = scoreSeq[0];
		alignProbability = scoreSeq[1];
		chFreq = atoi(scoreSeq[2].c_str());
		enFreq = atoi(scoreSeq[3].c_str());
		pairFrequency = atoi(scoreSeq[4].c_str());
		totalLine = atoi(scoreSeq[5].c_str());
		mu = atof(scoreSeq[6].c_str());
		cc = atof(scoreSeq[7].c_str());
		lr = atof(scoreSeq[8].c_str());
		dc = atof(scoreSeq[9].c_str());
		fc = atof(scoreSeq[10].c_str());
	}
	else{//New pair
		if(alignScore.find(".") != string::npos){alignProbability = alignScore;}
		else{alignFreq = alignScore;}
		//Get relative information
		pairFrequency = getPairFrequency(chWord, alignWord, chContext, enContext, chFreq, enFreq, totalLine, fc);
		if(pairFrequency < 1){return false;}
		//calculate relative parameter
		mu = getMutualInformation(totalLine, chFreq, enFreq, pairFrequency);
		cc = getCorrelationCoefficient(totalLine, chFreq, enFreq, pairFrequency);
		lr = getLikehoodRatios(totalLine, chFreq, enFreq, pairFrequency);
		dc = getDice(totalLine, chFreq, enFreq, pairFrequency);
	}
	//Merge and output
	alignScore = alignFreq + "," + alignProbability + "," + int2str(chFreq) + "," + int2str(enFreq) + "," + int2str(pairFrequency) + "," + int2str(totalLine) + "," + double2str(mu) + "," + double2str(cc) + "," + double2str(lr) + "," + double2str(dc) + "," + double2str(fc);
	alignResult[tmpStr] = alignScore;

	return true;
}

bool testFun(int idx,map<string, string> &alignResult ,vector<string> &chContext, vector<string> &enContext){
	int i;
	for(i = 0; i < 1000000; i++){}
	//cout << idx << endl;
}
