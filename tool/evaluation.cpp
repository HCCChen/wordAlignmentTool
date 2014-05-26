/*
	Author: Paul Chen
	Date: 2014/3/4
	Target: Evaluate Word Alignment result
*/
#include "library.h"

typedef struct ALIGNLIB{//For known word Lib
	string enWord[32];
	int alignCount;
	int useFlag;
}alignLib;

typedef struct ALIGN_INFO{//For unknown word pair
	vector<string> enWord;	//For each pair
	vector<int> pairFreq;
	vector<double> mu;
	vector<double> cc;
	vector<double> lr;
	vector<double> dc;
	vector<double> fc;
}alignInfo;

bool updateAlignInfo(map<string, alignInfo> &wordPairInfo, vector<string> &singlePairInfo);
string getBestAlignByDC(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByCC(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByLR(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByMU(string chWord, map<string, alignInfo> &pairInfo);

int main(int argc, char* argv[]){
	const string KNOWN_ALIGN_LIB_PATH = "../data/knownWordAlign20140522";
	const string PART_ERROR_RESULT_PATH = "../data/errorRecordPart";
	const string FULL_ERROR_RESULT_PATH = "../data/errorRecordFull";
	const string MISS_ALIGN_RESULT_PATH = "../data/errorRecordWithoutError";
	const double FILT_PROBABILITY = 0.2;
	const int FILT_ALIGNMENT_FREQUENCY = 3;//More than ...
	const int FILT_CONTEXT_FREQUENCY = 0;//More than ...
	const double FILT_DICE_VALUE = 0.2;//More than ..
	const bool FILTER_SWITCH = false;//true = do filter
	const int NUMBER_OF_FINAL_ALIGN = 5;
	string ALIGN_PATH;
	string EVALUTE_RESULT_PATH;
	if(argc == 3){
		ALIGN_PATH.assign(argv[1]);
		EVALUTE_RESULT_PATH.assign(argv[2]);
	}
	else{
		ALIGN_PATH = "../data/ngramPairResult";
		EVALUTE_RESULT_PATH = "../data/evaluteResult";
	}
	map<string, alignLib> wordLib;
	map<string, alignInfo> wordPairInfo;
	map<string, alignInfo>::iterator iter;
	vector<string> candidateSeg;
	vector<string> resultPool;
	fstream fin, fout, ferr1, ferr2;
	char buf[4096];
	string tmpStr, chWord, alignWord;
	int partErrorFlag, i ,j , filtFlag, alignCountChWord = 0, singleWord = 0, phraseWord = 0, numberOfFinalResult = 0;
	int correctAlign = 0, partErrorAlign = 0, fullErrorAlign = 0 , missAlign = 0, dropAlign = 0 ,totalAlign = 0, alignLibCount = 0, usedAlign = 0;
	double gizaFreq, gizaProbability, pairFreq, diceValue;
	double precisionRate, recallRate;
	//load align lib
	cout << "\E[1;32;40mPreparing!\E[0m" << endl;
	fin.open(KNOWN_ALIGN_LIB_PATH.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		for(i = 0; i < strlen(buf); i++){
			if(buf[i] >= 'a' && buf[i] <= 'z'){//to lower
				buf[i] += 'a' - 'a';
			}
		}
		tmpStr.assign(buf);
		if(tmpStr.length() < 3){continue;}
		alignLibCount++;
		chWord = tmpStr.substr(0, tmpStr.find(','));
		alignWord = tmpStr.substr(tmpStr.find(',')+1);
		//Put in the word lib
		if(wordLib.find(chWord) == wordLib.end()){//New word
			alignCountChWord++;
			wordLib[chWord].enWord[0] = alignWord;
			wordLib[chWord].alignCount = 1;
			wordLib[chWord].useFlag = 0;
		}
		else{
			wordLib[chWord].enWord[wordLib[chWord].alignCount] = alignWord;
			wordLib[chWord].alignCount++;
		}
	}
	fin.close();
	
	//Judge Result
	cout << "\E[1;32;40mLoad all pair, please wait a minute...\E[0m" << endl;
	fin.open(ALIGN_PATH.c_str(), ios::in);
	fout.open(EVALUTE_RESULT_PATH.c_str(), ios::out);
	ferr1.open(PART_ERROR_RESULT_PATH.c_str(), ios::out);
	ferr2.open(FULL_ERROR_RESULT_PATH.c_str(), ios::out);
	while(!fin.eof()){//For each Align Result
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.length() < 3){continue;}
		filtFlag = 0;
		//Divide Word
		totalAlign++;
		explode(',', tmpStr, candidateSeg);
		updateAlignInfo(wordPairInfo, candidateSeg);
	}
	//Find Best align for each chWord
	cout << "\E[1;32;40mGet best align pair\E[0m" << endl;
	for(iter = wordPairInfo.begin(); iter != wordPairInfo.end(); iter++){
		tmpStr = iter->first;
		for(i = 0; i < NUMBER_OF_FINAL_ALIGN; i++){//Find top N result
			resultPool.push_back(getBestAlignByLR(tmpStr, wordPairInfo));
		}
		numberOfFinalResult += NUMBER_OF_FINAL_ALIGN;
	}

	cout << "\E[1;32;40mEvaluate result...\E[0m" << endl;
	//-----------evaluation candidate and record it----------
	for(i = 0; i < resultPool.size(); i++){
		chWord = resultPool[i].substr(0, resultPool[i].find(","));
		alignWord = resultPool[i].substr(resultPool[i].find(",")+1);
		if(wordLib[chWord].useFlag == 0){
			wordLib[chWord].useFlag = 1;
			usedAlign += (wordLib[chWord].alignCount);
		}
		for(j = 0, partErrorFlag = -1; j < wordLib[chWord].alignCount; j++){//for each align lib
			if(trim(wordLib[chWord].enWord[j]) == trim(alignWord)){//correct align
				if(alignWord.find(' ') != string::npos){phraseWord++;}
				else{singleWord++;}
				correctAlign++;
				wordLib[chWord].enWord[j] = "";
				partErrorFlag = -2;
				fout << chWord << "," << alignWord << endl;
				break;
			}
			if(wordLib[chWord].enWord[j].find(alignWord) != string::npos){
				partErrorFlag = j;
			}
		}
	}

	//Output evalute result
	fullErrorAlign = totalAlign - correctAlign - missAlign - partErrorAlign - dropAlign;
	precisionRate = (double)correctAlign*100/(double)(numberOfFinalResult);
	recallRate = (double)correctAlign*100/(double)(alignLibCount);
	fout << "==============================" << endl;
	fout << "Known Library count: " << alignLibCount << " Pairs" << endl;
	fout << "Total Chinese Align Word: " << alignCountChWord << " Pairs" << endl;
	fout << "Used Chinese Align Count: " << usedAlign << " Pairs" << endl;
	fout << "Total Align Word: " << totalAlign << " words"  << endl;
	fout << "Total Align Word: " << totalAlign << " words"  << endl;
	fout << "Correct Align Word: " << correctAlign << "(" << singleWord << "+" << phraseWord << ") words"<< endl;
	fout << "Part Error Align Word: " << partErrorAlign << " words" << endl;
	fout << "Full Error Align Word: " << fullErrorAlign << " words" << endl;
	fout << "Drop Error Align Word: " << dropAlign << " words" << endl;
	fout << "Miss Align Word: " << missAlign << " words" << endl;
	fout << "Alignment Precision Rate(correctAlign/Not Miss Align): " << precisionRate << "%" << endl;
	fout << "Alignment Recall Rate(correctAlign/Align in library): " << recallRate << "%" << endl;
	fout << "Alignment Recall Rate(correctAlign/Align be used in library): " << (double)correctAlign*100/(double)(usedAlign) << "%" << endl;
	fout << "f-measure: " << (2*precisionRate*recallRate)/(precisionRate + recallRate) << "%" << endl;
	fin.close();
	fout.close();

	
	
	return 0;
}

bool updateAlignInfo(map<string, alignInfo> &wordPairInfo, vector<string> &singlePairInfo){
	string chWord, enWord;
	int i;
	chWord = singlePairInfo[0];
	enWord = singlePairInfo[1];
	for(i = 5; i <=8; i++ ){
		if(singlePairInfo[i].find("-") != string::npos){return false;}
	}
	wordPairInfo[chWord].enWord.push_back(enWord);
	wordPairInfo[chWord].pairFreq.push_back(str2int(singlePairInfo[4]));
	wordPairInfo[chWord].mu.push_back(atof(singlePairInfo[5].c_str()));
	wordPairInfo[chWord].cc.push_back(atof(singlePairInfo[6].c_str()));
	wordPairInfo[chWord].lr.push_back(atof(singlePairInfo[7].c_str()));
	wordPairInfo[chWord].dc.push_back(atof(singlePairInfo[8].c_str()));
	wordPairInfo[chWord].fc.push_back(0);
	return true;
}

string getBestAlignByDC(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<double>::iterator pos = max_element(pairInfo[chWord].dc.begin(), pairInfo[chWord].dc.end());
	double dcValue = *pos;
	int idx = find(pairInfo[chWord].dc.begin(), pairInfo[chWord].dc.end(), dcValue) - pairInfo[chWord].dc.begin();
	pairInfo[chWord].dc[idx] = -1;
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	return tmpStr;
}

string getBestAlignByCC(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<double>::iterator pos = max_element(pairInfo[chWord].cc.begin(), pairInfo[chWord].cc.end());
	double dcValue = *pos;
	int idx = find(pairInfo[chWord].cc.begin(), pairInfo[chWord].cc.end(), dcValue) - pairInfo[chWord].cc.begin();
	pairInfo[chWord].cc[idx] = -1;
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	return tmpStr;
}

string getBestAlignByMU(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<double>::iterator pos = max_element(pairInfo[chWord].mu.begin(), pairInfo[chWord].mu.end());
	double dcValue = *pos;
	int idx = find(pairInfo[chWord].mu.begin(), pairInfo[chWord].mu.end(), dcValue) - pairInfo[chWord].mu.begin();
	pairInfo[chWord].mu[idx] = -1;
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	return tmpStr;
}

string getBestAlignByLR(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<double>::iterator pos = max_element(pairInfo[chWord].lr.begin(), pairInfo[chWord].lr.end());
	double dcValue = *pos;
	int idx = find(pairInfo[chWord].lr.begin(), pairInfo[chWord].lr.end(), dcValue) - pairInfo[chWord].lr.begin();
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	pairInfo[chWord].lr[idx] = -1;
	return tmpStr;
}
