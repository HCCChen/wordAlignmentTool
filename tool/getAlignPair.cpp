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
int generateWordPair(vector<string> chSentecneSeg, vector<string> nGram, map<string, int> &wordPair, map<string, double> &wordPairWithFC, map<string, int> &specialLib);
//Generate EM Value
bool generateEMValue(map<string, int> &wordPair, map<string, double> &wordPairWithFC, map<string, map<string, double>> &wordP_C_EByChWord, const int iteration);


int main(int argc, char* argv[]){
	const int FILTER_OF_FREQUENCY = 1;//最少須出現次數
	const int ITERATION_NUMBER = 0;
	const int MAX_GRAM = 4;
	const double ALIGNMENT_PROBABILITY = 0;
	const string CH_CONTEXT_PATH = "../data/chBase";
	const string EN_CONTEXT_PATH = "../data/enBaseTmp";
//	const string CH_CONTEXT_PATH = "../data/chTest";
//	const string EN_CONTEXT_PATH = "../data/enTest";
	const string CH_KNOWNWORD_LIB = "../data/knownChWord";
	const string CH_SPECIAL_LIB_PATH = "../data/languageBase/specialWordLib";
	const string EN_SPECIAL_LIB_PATH = "../data/languageBase/specialWordLibEn";
	const string OUTPUT_PATH = "../data/ngramPairResult";
	char buf[4096];
	vector<string> chSentecneSeg;
	vector<string> enSentecneSeg;
	vector<string> knownChWord;
	vector<string> nGram;
	map<string, int> specialLibCh;
	map<string, int> specialLib;
	map<string, int> wordPair;
	map<string, double> wordPairWithFC;
	map<string, double> wordPairWithEM;
	map<string, int> chWordInfo;//Record chWord and Frequency
	map<string, int> enWordInfo;//Record enWord and Frequency
	map<string, int>::iterator iter;
	map<string, map<string, double>> wordP_C_EByChWord;
	int i,j, chFreq, enFreq, pairFreq, totalNumber = 0, totalPair = 0;
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
				if(chWordInfo.find(knownChWord[i]) == chWordInfo.end()){//Record fequency of chWord
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
		totalPair += generateWordPair(chSentecneSeg, nGram, wordPair,wordPairWithFC ,specialLibCh);
		cout << totalNumber << " : " << wordPairWithFC.size() << " pairs, total have " << enWordInfo.size() << " english words" << endl;
	}
	fin.close();
	fin2.close();
	cout << "\E[1;32;40mHave Generate " << totalPair << " pairs from " << totalNumber << " lines\E[0m" << endl;

	cout << "\E[1;32;40mGenerate EM value by Expectation-maximization algorithm \E[0m" << endl;
	generateEMValue(wordPair , wordPairWithFC, wordP_C_EByChWord, ITERATION_NUMBER);
	//Output
	cout << "\E[1;32;40mOutput word pair,total have " << wordPair.size() << " pairs\E[0m" << endl;
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	for(i = 0, iter = wordPair.begin();iter != wordPair.end(); iter++, i++){
		if(iter->second < FILTER_OF_FREQUENCY){continue;}
		tmpStr = iter->first;
		chWord = tmpStr.substr(0, tmpStr.find(","));
		enWord = tmpStr.substr(tmpStr.find(",")+1);
		pairFreq = iter->second;
		chFreq = chWordInfo[chWord]-pairFreq;
		enFreq = enWordInfo[enWord]-pairFreq;
		mu = getMutualInformation(totalNumber, chFreq, enFreq, pairFreq);
		cc = getCorrelationCoefficient(totalNumber, chFreq, enFreq, pairFreq);
		lr = getLikehoodRatios(totalNumber, chFreq, enFreq, pairFreq);
		dc = getDice(totalNumber, chFreq, enFreq, pairFreq);
		fc = wordPairWithFC[tmpStr];

		//Basic Filter	
		if(mu < 0 || lr < 0 || cc < 0 || dc < 0 || pairFreq < FILTER_OF_FREQUENCY){continue;}
		if(wordP_C_EByChWord[chWord][enWord] < ALIGNMENT_PROBABILITY){continue;}
		cout << i << "," << chWord << "," << enWord << endl;
		fout << chWord << "," << enWord << "," << chFreq << "," << enFreq << ","  << pairFreq << "," << mu  << "," << cc << "," << lr << "," << dc << "," << fc << "," << wordP_C_EByChWord[chWord][enWord] << endl;
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
			}
		}
	}
	//Remove Repeat item
	sort(nGram.begin(), nGram.end());
	nGram.erase( unique( nGram.begin(), nGram.end() ), nGram.end());
	//Record Information
	for(i = 0; i < nGram.size(); i++){
		tmpStr = nGram[i];
		if(enWordInfo.find(tmpStr) == enWordInfo.end()){enWordInfo[tmpStr] = 1;}
		else{enWordInfo[tmpStr]++;}
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
	if(wordSeg[0] == "of" || wordSeg[0] == "off" || wordSeg[0] == "and"
	|| wordSeg[0] == "or" || wordSeg[0] == "but" || wordSeg[0] == "nor"
	|| wordSeg[0] == "i" || wordSeg[0] == "he" || wordSeg[0] == "she"
	|| wordSeg[0] == "his" || wordSeg[0] == "her" || wordSeg[0] == "him"
	|| wordSeg[0] == "we" || wordSeg[0] == "they" || wordSeg[0] == "them"
	|| wordSeg[0] == "were" || wordSeg[0] == "have"
    || wordSeg[0] == "has" || wordSeg[0] == "may" || wordSeg[0] == "when"
    || wordSeg[0] == "what" || wordSeg[0] == "where" || wordSeg[0] == "why"
    || wordSeg[0] == "who" || wordSeg[0] == "how"){
		return false;
	}
	
	if(wordSeg[t] == "and" || wordSeg[t] == "or" || wordSeg[t] == "but"
	|| wordSeg[t] == "his" || wordSeg[t] == "her" || wordSeg[t] == "it"
	|| wordSeg[t] == "i" || wordSeg[t] == "they" || wordSeg[t] == "them"
	|| wordSeg[t] == "am" || wordSeg[t] == "are" || wordSeg[t] == "is"
	|| wordSeg[t] == "no" || wordSeg[t] == "not" || wordSeg[t] == "have"
	|| wordSeg[t] == "has" || wordSeg[t] == "may" || wordSeg[t] == "when"
	|| wordSeg[t] == "what" || wordSeg[t] == "where" || wordSeg[t] == "why"
	|| wordSeg[t] == "who" || wordSeg[t] == "how" || wordSeg[t] == "the"
	|| wordSeg[t] == "an" || wordSeg[t] == "a" || wordSeg[t] == "be"){
		return false;
	}
	return true;
}

int generateWordPair(vector<string> chSentecneSeg, vector<string> nGram, map<string, int> &wordPair,map<string, double> &wordPairWithFC ,map<string, int> &specialLib){
	int i,j,reti , totalPair = 0;
	string tmpStr;
	regex_t regexLanguage;
	reti = regcomp(&regexLanguage, "[a-zA-Z0-9]", 0);
	for(i = 0; i < chSentecneSeg.size(); i++){//For each chinese word
		//Chinese filter
		if(chSentecneSeg[i].length() < 3||specialLib.find(chSentecneSeg[i]) != specialLib.end()){continue;}
		reti = regexec(&regexLanguage, chSentecneSeg[i].c_str(), 0, NULL, 0);
		if( !reti ){//中文文句包含英文數字:
		//	continue;
		}
		//Generate pair
		for(j = 0; j < nGram.size(); j++){//For each gram
			if(nGram[j].length() < 2){continue;}
			tmpStr = chSentecneSeg[i] + "," + nGram[j];
			if(wordPair.find(tmpStr) == wordPair.end()){
				totalPair++;
				wordPair[tmpStr] = 1;
				wordPairWithFC[tmpStr] = 1/(double)(nGram.size());
			}
			else{
				wordPair[tmpStr]++;
				wordPairWithFC[tmpStr] += 1/(double)(nGram.size());
			}
		}
	}
	return totalPair;
}

bool generateEMValue(map<string, int> &wordPair, map<string, double> &wordPairWithFC, map<string, map<string, double>> &wordP_C_EByChWord, const int iteration){
	map<string, map<string, double>> wordS_C_EByChWord;
	map<string, map<string, int>>::iterator chPos;
	map<string, map<string, double>>::iterator infoPos;
	map<string, int> pairFreq;
	map<string, double> pairInfo;
	map<string, int>::iterator pos;
	map<string, double>::iterator wordInfoPos;
	string tmpStr, chWord, enWord;
	int f11, i;
	double scvValue, sceValue, fcValue;
	if(iteration == 0){return true;}
	//initialize
	cout << "\E[1;34;40m\tInitialize...\E[0m" << endl;
	for(pos = wordPair.begin(); pos != wordPair.end(); pos++){
		tmpStr = pos->first;
		f11 = pos->second;
		chWord = tmpStr.substr(0, tmpStr.find(","));
		enWord = tmpStr.substr(tmpStr.find(",")+1);
		wordP_C_EByChWord[chWord][enWord] = 1;
		wordS_C_EByChWord[chWord][enWord] = 0;
	}
/*
	for(wordInfoPos = wordPairWithFC.begin(); wordInfoPos != wordPairWithFC.end(); wordInfoPos++){
		tmpStr = wordInfoPos->first;
		fcValue = wordInfoPos->second;
		chWord = tmpStr.substr(0, tmpStr.find(","));
		enWord = tmpStr.substr(tmpStr.find(",")+1);
		wordP_C_EByChWord[chWord][enWord] = fcValue;
	}
*/
	for(i = 1; i <= iteration; i++){
		cout << "\E[1;34;40m\tEM iteration: " << i << "\E[0m" << endl;
		//E-step: Generate S(c.e) value
		for(infoPos = wordS_C_EByChWord.begin(); infoPos != wordS_C_EByChWord.end(); infoPos++){//For each chWord
			chWord = infoPos->first;
			pairInfo = infoPos->second;
			for(wordInfoPos = pairInfo.begin(); wordInfoPos != pairInfo.end(); wordInfoPos++){//For each pair
				enWord = wordInfoPos->first;
				tmpStr = chWord + "," + enWord;
				wordS_C_EByChWord[chWord][enWord] = wordP_C_EByChWord[chWord][enWord]*wordPair[tmpStr];
			}
		}
		//M-step: Generate P(c,e) value
		for(infoPos = wordS_C_EByChWord.begin(); infoPos != wordS_C_EByChWord.end(); infoPos++){//For each chWord
			chWord = infoPos->first;
			pairInfo = infoPos->second;
			for(scvValue = 0, wordInfoPos = pairInfo.begin(); wordInfoPos != pairInfo.end(); wordInfoPos++){
				//Get sum of s(c,e) of this chWord
				scvValue += wordInfoPos->second;
			}
			for(wordInfoPos = pairInfo.begin(); wordInfoPos != pairInfo.end(); wordInfoPos++){
				enWord = wordInfoPos->first;
				sceValue = wordInfoPos->second;
				wordP_C_EByChWord[chWord][enWord] = sceValue/(scvValue-sceValue);
			}
		}
	}

	return true;
}