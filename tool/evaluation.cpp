/*
Author: Paul Chen
Date: 2014/3/4
Target: Evaluate Word Alignment result
 */
#include "library.h"

typedef struct ALIGNLIB{//For known word Lib
	string enWord[32];
	int enWordUsedFlag[32];
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
	vector<double> ap; //Alignment Probability
	vector<double> sumOfParameter;
}alignInfo;

bool updateAlignInfo(map<string, alignInfo> &wordPairInfo, vector<string> &singlePairInfo);
string getBestAlignByDC(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByCC(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByLR(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByMU(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByFC(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByAP(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignBySUM(string chWord, map<string, alignInfo> &pairInfo);
string getBestAlignByFreq(string chWord, map<string, alignInfo> &pairInfo);
bool vector_cmp_by_dc(vector<string> a, vector<string> b);

const double THRESHOLD_OF_PROBABILITY = 0.001;

int main(int argc, char* argv[]){
	const string KNOWN_ALIGN_LIB_PATH = "../data/knownWordAlign20140522";
	const string PART_ERROR_RESULT_PATH = "../data/errorRecordPart";
	const string FULL_ERROR_RESULT_PATH = "../data/errorRecordFull";
	const string MISS_ALIGN_RESULT_PATH = "../data/errorRecordWithoutError";
	const string EN_PREFIX_PATH = "../data/languageBase/enPrefix";
	const string EN_POSTFIX_PATH = "../data/languageBase/enPostfix";
	const string ENLIB_PATH = "../data/enBase.xml";
	const bool FILTER_SWITCH = true;//true = do filter
	const bool EVALUATE_SWITCH = false;//true = do evaluate; false = just output
	const bool USE_GLOBAL_INFO = false;
	const bool USE_LOCAL_INFO = true;
	const int NUMBER_OF_FINAL_ALIGN = 5;
	string ALIGN_PATH;
	string EVALUTE_RESULT_PATH;
	if(argc == 3){
		ALIGN_PATH.assign(argv[1]);
		EVALUTE_RESULT_PATH.assign(argv[2]);
	}
	else{
		ALIGN_PATH = "../data/ngramPairResult";
		//ALIGN_PATH = "../data/supportAlign";
		EVALUTE_RESULT_PATH = "../data/evaluteResult";
	}
	map<string, alignLib> wordLib;
	map<string, alignInfo> wordPairInfo;
	map<string, alignInfo>::iterator iter;
	map<string, string> lemmaPair;
	vector<vector<string>> pairList;
	vector<string> candidateSeg;
	vector<string> resultPool;
	vector<string> enPrefix;
	vector<string> enPostfix;
	fstream fin, fout, ferr1, ferr2;
	char buf[4096];
	string tmpStr, chWord, alignWord, enBuf1, enBuf2, baseWord, lemmaWord;
	int i, j, k, filtFlag, alignCountChWord = 0, singleWord = 0, phraseWord = 0, numberOfFinalResult = 0, partCorrect = 0, partCorrectFlag = 0, partUsedAlign = 0;
	int correctAlign = 0, partErrorAlign = 0, fullErrorAlign = 0 , missAlign = 0, dropAlign = 0 ,totalAlign = 0, alignLibCount = 0, usedAlign = 0;
	int pos1, pos2, singleWordPart = 0, phraseWordPart = 0;
	double gizaFreq, gizaProbability, pairFreq, diceValue;
	double precisionRate, recallRate, recallRateForCandidate, precisionRateForGC;

	//load align lib
	cerr << "\E[1;32;40mPreparing!\E[0m" << endl;
	loadFile(EN_PREFIX_PATH, enPrefix);
	loadFile(EN_POSTFIX_PATH, enPostfix);
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
			wordLib[chWord].enWordUsedFlag[0] = 0;
			wordLib[chWord].alignCount = 1;
			wordLib[chWord].useFlag = 0;
		}
		else{
			wordLib[chWord].enWord[wordLib[chWord].alignCount] = alignWord;
			wordLib[chWord].enWordUsedFlag[wordLib[chWord].alignCount] = 0;
			wordLib[chWord].alignCount++;
		}
	}
	fin.close();

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



	//Judge Result
	cerr << "\E[1;32;40mLoad all pair, please wait a minute...\E[0m" << endl;
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
		candidateSeg.clear();
		explode(',', tmpStr, candidateSeg);
		if(FILTER_SWITCH == false){
			tmpStr = candidateSeg[0] + "," + candidateSeg[1];
			resultPool.push_back(tmpStr);
		}
		else if(USE_LOCAL_INFO == true){
			updateAlignInfo(wordPairInfo, candidateSeg);	//For local information
		}
		else if(USE_GLOBAL_INFO == true){
			pairList.push_back(candidateSeg);	//For global information
		}
	}

	//Find Best align for each chWord
	if(USE_LOCAL_INFO == true && FILTER_SWITCH == true){
		cerr << "\E[1;32;40mGet best align pair for each ChWord\E[0m" << endl;
		for(iter = wordPairInfo.begin(); iter != wordPairInfo.end(); iter++){//for each chWord
			tmpStr = iter->first;
			for(i = 0; i < NUMBER_OF_FINAL_ALIGN; i++){//Find top N result
				resultPool.push_back(getBestAlignByDC(tmpStr, wordPairInfo));
				resultPool.push_back(getBestAlignByCC(tmpStr, wordPairInfo));
				resultPool.push_back(getBestAlignByLR(tmpStr, wordPairInfo));
				resultPool.push_back(getBestAlignByFC(tmpStr, wordPairInfo));
				//resultPool.push_back(getBestAlignByMU(tmpStr, wordPairInfo));
				//resultPool.push_back(getBestAlignByAP(tmpStr, wordPairInfo));
				//resultPool.push_back(getBestAlignBySUM(tmpStr, wordPairInfo));
				//resultPool.push_back(getBestAlignByFreq(tmpStr, wordPairInfo));
			}
		}
	}
	if(USE_GLOBAL_INFO == true && FILTER_SWITCH == true){
		cerr << "\E[1;32;40mGet best align pair from all path\E[0m" << endl;
		sort(pairList.begin(), pairList.end(), vector_cmp_by_dc);
		for(i = 0; i < 10690; i++){
			tmpStr = pairList[i][0] + "," + pairList[i][1];
			resultPool.push_back(tmpStr);
		}
	}

	//Remove Repeat item
	sort(resultPool.begin(), resultPool.end());
	resultPool.erase( unique( resultPool.begin(), resultPool.end() ), resultPool.end());
	numberOfFinalResult = resultPool.size();


	//Evalute and output result
	if(EVALUATE_SWITCH == false){
		cerr << "\E[1;32;40mJust Output candidate pair\E[0m" << endl;
		for(i = 0, partErrorAlign = 0, fullErrorAlign = 0; i < resultPool.size(); i++){//For each Pair
			fout << resultPool[i] << endl;
		}
	}
	else if(EVALUATE_SWITCH == true){
		cerr << "\E[1;32;40mEvaluate result...\E[0m" << endl;
		//-----------evaluation candidate and record it----------
		for(i = 0, partErrorAlign = 0, fullErrorAlign = 0; i < resultPool.size(); i++){//For each Pair
			chWord = resultPool[i].substr(0, resultPool[i].find(","));
			alignWord = resultPool[i].substr(resultPool[i].find(",")+1);
			if(wordLib[chWord].useFlag == 0){//calculate "usedAlign"
				wordLib[chWord].useFlag = 1;
				usedAlign += (wordLib[chWord].alignCount);
			}
			for(j = 0, partCorrectFlag = -1; j < wordLib[chWord].alignCount; j++){//for each align lib
				if(trim(wordLib[chWord].enWord[j]) == trim(alignWord)){//correct align
					if(alignWord.find(' ') != string::npos){phraseWord++;}
					else{singleWord++;}
					if(wordLib[chWord].enWordUsedFlag[j] == -1){
						partUsedAlign--;
					}
					wordLib[chWord].enWordUsedFlag[j] = 1;//is be used in "all correct"
					correctAlign++;
					partCorrectFlag = -1;
					wordLib[chWord].enWord[j] = "";	//Not repeat result
					fout << chWord << "," << alignWord << endl;
					break;
				}
				if((wordLib[chWord].enWord[j].find(alignWord) != string::npos
							|| alignWord.find(wordLib[chWord].enWord[j]) != string::npos)
						&& wordLib[chWord].enWord[j].length() > 2){//Could be Part correct
					if(alignWord.find(" ") == string::npos && wordLib[chWord].enWord[j].find(" ") == string::npos){//For single word
						if(lemmaPair.find(alignWord) != lemmaPair.end()){
							enBuf1 = lemmaPair[alignWord];	
						}
						else{enBuf1 = alignWord;}
						if(lemmaPair.find(wordLib[chWord].enWord[j]) != lemmaPair.end()){
							enBuf2 = lemmaPair[wordLib[chWord].enWord[j]];
						}
						else{enBuf2 = wordLib[chWord].enWord[j];}
						if(enBuf1 == enBuf2 && partCorrectFlag == -1){partCorrectFlag = j;}
					}
					else{
						//for pre-fix
						enBuf1 = alignWord;
						enBuf2 = wordLib[chWord].enWord[j];
						for(k = 0; k < enPrefix.size(); k++){
							tmpStr = enPrefix[k]+" ";
							if(enBuf1.find(tmpStr) == 0){enBuf1 = enBuf1.substr(enBuf1.find(" ")+1);}
							if(enBuf2.find(tmpStr) == 0){enBuf2 = enBuf2.substr(enBuf2.find(" ")+1);}
							if(enBuf1 == enBuf2 && partCorrectFlag == -1){partCorrectFlag = j;}
						}
						//for post-fox
						enBuf1 = alignWord;
						enBuf2 = wordLib[chWord].enWord[j];
						for(k = 0; k < enPostfix.size(); k++){
							if(enBuf1.substr(enBuf1.find_last_of(" ")+1) == enPostfix[k]){
								enBuf1 = enBuf1.substr(0, enBuf1.find_last_of(" "));
							}
							if(enBuf2.substr(enBuf2.find_last_of(" ")+1) == enPostfix[k]){
								enBuf2 = enBuf2.substr(0, enBuf2.find_last_of(" "));
							}
							if(enBuf1 == enBuf2 && partCorrectFlag == -1){partCorrectFlag = j;}
						}
					}
				}
			}
			if(partCorrectFlag != -1){
				if(wordLib[chWord].enWordUsedFlag[partCorrectFlag] != 1){//Record Part Correct infoamtion
					partUsedAlign++;
					wordLib[chWord].enWordUsedFlag[partCorrectFlag] = -1;
				}

				if(alignWord.find(' ') != string::npos){phraseWordPart++;}
				else{singleWordPart++;}
				partCorrect++;
				ferr1 << chWord << "," << alignWord << "," << wordLib[chWord].enWord[partCorrectFlag] << endl;
			}
		}

		//Output evalute result
		fullErrorAlign = numberOfFinalResult - correctAlign - missAlign - partErrorAlign - dropAlign;
		precisionRate = (double)correctAlign*100/(double)(numberOfFinalResult);
		precisionRateForGC = (double)(correctAlign+partCorrect)*100/(double)(numberOfFinalResult);
		recallRate = (double)correctAlign*100/(double)(alignLibCount);
		recallRateForCandidate = (double)correctAlign*100/(double)(usedAlign);
		fout << "==============================" << endl;
		fout << "Known Library count: " << alignLibCount << " Pairs" << endl;
		fout << "Total Chinese Align Word: " << alignCountChWord << " Pairs" << endl;
		fout << "Used Word Align Count: " << usedAlign << " Pairs" << endl;
		fout << "Total Align Word: " << totalAlign << " words"  << endl;
		fout << "All candidate word: " << numberOfFinalResult << " words"  << endl;
		fout << "All Correct Align Word: " << correctAlign << "(" << singleWord << "+" << phraseWord << ") words"<< endl;
		fout << "Part Correct Align Word: " << partCorrect << "(" << singleWordPart << "+" << phraseWordPart << ") words" << endl;
		fout << "Part Correct Align Used Align: " << partUsedAlign << " words" << endl;
		fout << "Full Error Align Word: " << fullErrorAlign << " words" << endl;
		fout << "Drop Error Align Word: " << dropAlign << " words" << endl;
		fout << "Miss Align Word: " << missAlign << " words" << endl;
		fout << "Alignment Precision Rate((All)correctAlign/Not Miss Align): " << precisionRate << "%" << endl;
		fout << "Alignment Precision Rate((Part+All)correctAlign/Not Miss Align): " << precisionRateForGC << "%" << endl;
		fout << "Alignment Recall Rate(correctAlign/Align in library): " << recallRate << "%" << endl;
		fout << "Alignment Recall Rate(correctAlign/Align be used in library): " << recallRateForCandidate << "%" << endl;
		recallRateForCandidate = (double)(correctAlign+partUsedAlign)*100/(double)(usedAlign);
		fout << "Alignment Recall Rate((part+all)correctAlign/Align be used in library): " << recallRateForCandidate << "%" << endl;
		fout << "f-measure: " << (2*precisionRate*recallRateForCandidate)/(precisionRate + recallRateForCandidate) << "%" << endl;
		fout << "f-measure(all+part): " << (2*precisionRateForGC*recallRateForCandidate)/(precisionRateForGC + recallRateForCandidate) << "%" << endl;
		fin.close();
		fout.close();
	}
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

	if(atof(singlePairInfo[10].c_str()) < THRESHOLD_OF_PROBABILITY){return false;}
	wordPairInfo[chWord].enWord.push_back(enWord);
	wordPairInfo[chWord].pairFreq.push_back(str2int(singlePairInfo[4]));
	wordPairInfo[chWord].mu.push_back(atof(singlePairInfo[5].c_str()));
	wordPairInfo[chWord].cc.push_back(atof(singlePairInfo[6].c_str()));
	wordPairInfo[chWord].lr.push_back(atof(singlePairInfo[7].c_str()));
	wordPairInfo[chWord].dc.push_back(atof(singlePairInfo[8].c_str()));
	wordPairInfo[chWord].fc.push_back(atof(singlePairInfo[9].c_str()));
	wordPairInfo[chWord].ap.push_back(atof(singlePairInfo[10].c_str()));
	wordPairInfo[chWord].sumOfParameter.push_back(atof(singlePairInfo[6].c_str()) + atof(singlePairInfo[7].c_str()) + atof(singlePairInfo[8].c_str()) + atof(singlePairInfo[5].c_str()) + atof(singlePairInfo[9].c_str()));
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

string getBestAlignByFC(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<double>::iterator pos = max_element(pairInfo[chWord].fc.begin(), pairInfo[chWord].fc.end());
	double dcValue = *pos;
	int idx = find(pairInfo[chWord].fc.begin(), pairInfo[chWord].fc.end(), dcValue) - pairInfo[chWord].fc.begin();
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	pairInfo[chWord].fc[idx] = -1;
	return tmpStr;
}

string getBestAlignByAP(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<double>::iterator pos = max_element(pairInfo[chWord].ap.begin(), pairInfo[chWord].ap.end());
	double dcValue = *pos;
	int idx = find(pairInfo[chWord].ap.begin(), pairInfo[chWord].ap.end(), dcValue) - pairInfo[chWord].ap.begin();
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	pairInfo[chWord].ap[idx] = -1;
	return tmpStr;
}


string getBestAlignBySUM(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<double>::iterator pos = max_element(pairInfo[chWord].sumOfParameter.begin(), pairInfo[chWord].sumOfParameter.end());
	double dcValue = *pos;
	int idx = find(pairInfo[chWord].sumOfParameter.begin(), pairInfo[chWord].sumOfParameter.end(), dcValue) - pairInfo[chWord].sumOfParameter.begin();
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	pairInfo[chWord].sumOfParameter[idx] = -1;
	return tmpStr;
}

string getBestAlignByFreq(string chWord, map<string, alignInfo> &pairInfo){
	string tmpStr = "";
	vector<int>::iterator pos = max_element(pairInfo[chWord].pairFreq.begin(), pairInfo[chWord].pairFreq.end());
	int dcValue = *pos;
	int idx = find(pairInfo[chWord].pairFreq.begin(), pairInfo[chWord].pairFreq.end(), dcValue) - pairInfo[chWord].pairFreq.begin();
	tmpStr = chWord + "," + pairInfo[chWord].enWord[idx];
	pairInfo[chWord].pairFreq[idx] = -1;
	return tmpStr;
}

bool vector_cmp_by_dc(vector<string> a, vector<string> b){
	return a[8] > b[8];//index of dice value
}
