/*
	Author: Paul Chen
	Date: 2014/05/20
	Target: 
		1. Get bilingual pair candidate for bilingual corpus
		2. Calculate statistic information
		3. The source of Chinese word: known lib
*/
#include "library.h"
using namespace std;

//Get n-gram and basic filter
bool generateNGram(vector<string> &sentenceSeg, vector<string> &nGram, int maxGram, map<string, int> &specialLib, map<string, int> &enWordInfo);
//Divide sentence by ',' ';' '.'
bool divideSentenceByAnchor(vector<string> &sentenceSeg, vector<vector<string>> &subSentence);
//For english filter
bool filtPhrase(string phrase);
//Pair chinese and english word and record it
bool generateWordPair(vector<string> chSentecneSeg, vector<string> nGram, map<string, int> &wordPair, map<string, int> &specialLib);

int main(int argc, char* argv[]){
	const int MAX_GRAM = 3;
	const string CH_CONTEXT_PATH = "../data/chBase";
	const string EN_CONTEXT_PATH = "../data/enBaseTmp";
	const string CH_KNOWNWORD_LIB = "../data/knownChWord";
	const string CH_SPECIAL_LIB_PATH = "../data/languageBase/specialWordLib";
	const string EN_SPECIAL_LIB_PATH = "../data/languageBase/specialWordLibEn";
	const string OUTPUT_PATH = "../data/ngramPairResult";
	const int FILTER_OF_FREQUENCY = 2;//最少須出現次數
	char buf[4096];
	vector<string> chSentecneSeg;
	vector<string> enSentecneSeg;
	vector<string> knownChWord;
	vector<string> nGram;
	map<string, int> specialLibCh;
	map<string, int> specialLib;
	map<string, int> wordPair;
	map<string, int> chWordInfo;//Record chWord and Frequency
	map<string, int> enWordInfo;//Record enWord and frequency
	map<string, int>::iterator iter;
	int i,j, chFreq, enFreq, pairFreq, totalNumber = 0;
	double mu = 0, cc = 0, lr = 0, dc = 0, fc = 0;
	string tmpStr, chLaw, enLaw, chWord, enWord;
	fstream fin, fin2, fout;

	//Load Basic Information
	loadFile(CH_SPECIAL_LIB_PATH, specialLibCh);
	loadFile(EN_SPECIAL_LIB_PATH, specialLib);
	loadFile(CH_KNOWNWORD_LIB, knownChWord);

	fin.open(CH_CONTEXT_PATH.c_str(), ios::in);
	fin2.open(EN_CONTEXT_PATH.c_str(), ios::in);
	cout << "\E[1;32;40mGenerating word pair by n-gram, please wait a minute...\E[0m" << endl;
	while(!fin.eof()){//For each sentence pair
		fin.getline(buf, 4096);
		chLaw.assign(buf);
		fin2.getline(buf, 4096);
		enLaw.assign(buf);
		if(chLaw.length() < 1 || enLaw.length() < 1){continue;}
		//Divide Chinese word
		totalNumber++;
		chSentecneSeg.clear();
		for(i = 0; i < knownChWord.size(); i++){//for each known chWord
			if(chLaw.find(knownChWord[i]) != string::npos){//For each chword which appear in this sentence
				chSentecneSeg.push_back(knownChWord[i]);
				if(chWordInfo.find(knownChWord[i]) == chWordInfo.end()){
					chWordInfo[knownChWord[i]] = 1;
				}
				else{
					chWordInfo[knownChWord[i]]++;
				}
			}
		}
		if(chSentecneSeg.size() == 0){continue;}
		//Get n-gram of english word
		explode(' ', enLaw, enSentecneSeg);
		generateNGram(enSentecneSeg, nGram, MAX_GRAM, specialLib, enWordInfo);
		generateWordPair(chSentecneSeg, nGram, wordPair, specialLibCh);
	}
	fin.close();
	fin2.close();

	//Output
	cout << "\E[1;32;40mOutput word pair...\E[0m" << endl;
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	for(iter = wordPair.begin();iter != wordPair.end(); iter++){
		if(iter->second < FILTER_OF_FREQUENCY){continue;}
		tmpStr = iter->first;
		chWord = tmpStr.substr(0, tmpStr.find(","));
		enWord = tmpStr.substr(tmpStr.find(",")+1);
		pairFreq = iter->second;
		chFreq = chWordInfo[chWord] - pairFreq;
		enFreq = enWordInfo[enWord] - pairFreq;
		fout << chWord << "," << enWord << "," << chFreq << "," << enFreq << ","  << pairFreq << endl;
	}
	fout.close();
	return 0;
}

bool generateNGram(vector<string> &sentenceSeg, vector<string> &nGram, int maxGram, map<string, int> &specialLib, map<string, int> &enWordInfo){
	int i, j,gramSize, subIdx;
	string tmpStr;
	vector<vector<string>> atomSentence;
	nGram.clear();
	divideSentenceByAnchor(sentenceSeg, atomSentence);
	for(gramSize = 1; gramSize <= maxGram; gramSize++){//For each gram size
		for(subIdx = 0; subIdx < atomSentence.size(); subIdx++){//For each Sub-sentence which divided by anchor
			for(i = 0; (i + gramSize) <= atomSentence[subIdx].size(); i++){//For each n-gram
				for(j = i, tmpStr = ""; j < i+gramSize; j++){//Merge to n-gram
					tmpStr += atomSentence[subIdx][j] + " ";
				}
				//Filter of n-gram
				tmpStr = trim(tmpStr);
				if(specialLib.find(tmpStr) != specialLib.end()){continue;}
				if(filtPhrase(tmpStr) == false){continue;}
				//Insert to library
				nGram.push_back(tmpStr);
				//Record Information
				if(enWordInfo.find(tmpStr) == enWordInfo.end()){
					enWordInfo[tmpStr] = 1;
				}
				else{
					enWordInfo[tmpStr]++;
				}
			}
		}
	}
	return true;
}

bool divideSentenceByAnchor(vector<string> &sentenceSeg, vector<vector<string>> &subSentence){
	int i, j;
	vector<string> basicVector;
	subSentence.clear();
	subSentence.push_back(basicVector);
	for(i = 0, j = 0; i < sentenceSeg.size(); i++){//For each seg
		if(sentenceSeg[i].find(",") != string::npos
		|| sentenceSeg[i].find(".") != string::npos
		|| sentenceSeg[i].find(";") != string::npos){
			subSentence[j].push_back(sentenceSeg[i].substr(0, sentenceSeg[i].length()-1));
			subSentence.push_back(basicVector);
			j++;
		}
		else{
			subSentence[j].push_back(sentenceSeg[i]);
		}
	}
	return true;
}

bool filtPhrase(string phrase){
	vector<string> wordSeg;
	int i,t;
	int reti;
	regex_t regexLanguage;
	reti = regcomp(&regexLanguage, "[0-9]", 0);
	explode(' ', phrase, wordSeg);

	if(phrase.length() < 3){return false;}

	reti = regexec(&regexLanguage, phrase.c_str(), 0, NULL, 0);
	if( !reti ){//文句包含數字:
		return false;
	}

	t = wordSeg.size()-1;
/*
	if(wordSeg[0] == "of" || wordSeg[0] == "off" || wordSeg[0] == "and"
	|| wordSeg[0] == "or" || wordSeg[0] == "but" || wordSeg[0] == "nor"
	|| wordSeg[0] == "i" || wordSeg[0] == "he" || wordSeg[0] == "she"
	|| wordSeg[0] == "his" || wordSeg[0] == "her" || wordSeg[0] == "him"
	|| wordSeg[0] == "we" || wordSeg[0] == "they" || wordSeg[0] == "them"
	|| wordSeg[0] == "be" || wordSeg[0] == "been" || wordSeg[0] == "am"
	|| wordSeg[0] == "are" || wordSeg[0] == "is" || wordSeg[0] == "was"
	|| wordSeg[0] == "were" || wordSeg[t] == "no" || wordSeg[t] == "not" || wordSeg[t] == "have"
    || wordSeg[t] == "has" || wordSeg[t] == "may" || wordSeg[t] == "when"
    || wordSeg[t] == "what" || wordSeg[t] == "where" || wordSeg[t] == "why"
    || wordSeg[t] == "who" || wordSeg[t] == "how"){
		return false;
	}
	if(wordSeg[t] == "and" || wordSeg[t] == "or" || wordSeg[t] == "but"
	|| wordSeg[t] == "am" || wordSeg[t] == "are" || wordSeg[t] == "is"
	|| wordSeg[t] == "no" || wordSeg[t] == "not" || wordSeg[t] == "have"
	|| wordSeg[t] == "has" || wordSeg[t] == "may" || wordSeg[t] == "when"
	|| wordSeg[t] == "what" || wordSeg[t] == "where" || wordSeg[t] == "why"
	|| wordSeg[t] == "who" || wordSeg[t] == "how" || wordSeg[t] == "the"
	|| wordSeg[t] == "an" || wordSeg[t] == "a" || wordSeg[t] == "be"){
		return false;
	}
*/
	return true;
}

bool generateWordPair(vector<string> chSentecneSeg, vector<string> nGram, map<string, int> &wordPair, map<string, int> &specialLib){
	int i,j;
	string tmpStr;
	int reti;
	regex_t regexLanguage;
	reti = regcomp(&regexLanguage, "[a-zA-Z0-9]", 0);
	for(i = 0; i < chSentecneSeg.size(); i++){
		//CChinese filter
		if(chSentecneSeg[i].length() < 3||specialLib.find(chSentecneSeg[i]) != specialLib.end()){continue;}
		reti = regexec(&regexLanguage, chSentecneSeg[i].c_str(), 0, NULL, 0);
		if( !reti ){//中文文句包含英文數字:
		//	continue;
		}
		//Generate pair
		for(j = 0; j < nGram.size(); j++){
			if(nGram[j].length() < 2){continue;}
			tmpStr = chSentecneSeg[i] + "," + nGram[j];
			if(wordPair.find(tmpStr) == wordPair.end()){
				wordPair[tmpStr] = 1;
			}
			else{
				wordPair[tmpStr]++;
			}
		}
	}
	return true;
}
