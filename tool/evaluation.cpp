/*
	Author: Paul Chen
	Date: 2014/3/4
	Target: Evaluate Word Alignment result
*/
#include "library.h"

typedef struct ALIGNLIB{
	string enWord[32];
	int alignCount;
	int useFlag;
}alignLib;

int main(int argc, char* argv[]){
	const string KNOWN_ALIGN_LIB_PATH = "../data/knownWordAlign20140518";
	//const string KNOWN_ALIGN_LIB_PATH = "../data/knownWordAlign";
	const string PART_ERROR_RESULT_PATH = "../data/errorRecordPart";
	const string FULL_ERROR_RESULT_PATH = "../data/errorRecordFull";
	const double FILT_PROBABILITY = 0.1;
	const int FILT_ALIGNMENT_FREQUENCY = 1;//More than ...
	const int FILT_CONTEXT_FREQUENCY = 0;//More than ...
	const double FILT_DICE_VALUE = 0.2;//More than ...
	const bool FILTER_SWITCH = false;//true = do filter
	string ALIGN_PATH;
	string EVALUTE_RESULT_PATH;

	if(argc == 3){
		ALIGN_PATH.assign(argv[1]);
		EVALUTE_RESULT_PATH.assign(argv[2]);
	}
	else{
		ALIGN_PATH = "../data/alignResult";
		EVALUTE_RESULT_PATH = "../data/evaluteResult";
	}
	map<string, alignLib> wordLib;
	vector<string> candidateSeg;
	fstream fin, fout, ferr1, ferr2;
	char buf[4096];
	string tmpStr, chWord, alignWord;
	int loopCount, partErrorFlag, i, filtFlag, alignCountChWord = 0, singleWord = 0, phraseWord = 0;
	int correctAlign = 0, partErrorAlign = 0, fullErrorAlign = 0 , missAlign = 0, dropAlign = 0 ,totalAlign = 0, alignLibCount = 0, usedAlign = 0;
	double gizaFreq, gizaProbability, pairFreq, diceValue;
	double precisionRate, recallRate;
	//load align lib
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
	fin.open(ALIGN_PATH.c_str(), ios::in);
	fout.open(EVALUTE_RESULT_PATH.c_str(), ios::out);
	ferr1.open(PART_ERROR_RESULT_PATH.c_str(), ios::out);
	ferr2.open(FULL_ERROR_RESULT_PATH.c_str(), ios::out);
	while(!fin.eof()){//For each Align Result
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		filtFlag = 0;
		//Divide Word
		totalAlign++;
		explode(',', tmpStr, candidateSeg);
		chWord = candidateSeg[0];
		alignWord = candidateSeg[1];
		gizaFreq = atof(candidateSeg[2].c_str());
		gizaProbability = atof(candidateSeg[3].c_str());
		pairFreq = atof(candidateSeg[6].c_str());
		diceValue = atof(candidateSeg[11].c_str());
		//-------------------Filter--------------------
		if(candidateSeg[3].find("e") != string::npos){
			filtFlag = 1;
		}
		else if((gizaFreq > 0 && gizaFreq < FILT_ALIGNMENT_FREQUENCY)
			|| (gizaProbability > 0 && gizaProbability < FILT_PROBABILITY)
			|| (pairFreq > 0 && pairFreq < FILT_CONTEXT_FREQUENCY)
			|| (diceValue > 0 && diceValue < FILT_DICE_VALUE)){
			filtFlag = 1;
		}
		//-----------Evaluation candidate and record it----------
		if(filtFlag == 1 && FILTER_SWITCH == true){
			dropAlign++;
			continue;
		}
		else if(wordLib.find(chWord) == wordLib.end()){//Miss align
			missAlign++;
			continue;
		}
		else{//Has Align
			if(wordLib[chWord].useFlag == 0){
				wordLib[chWord].useFlag = 1;
				usedAlign++;
			}
			for(loopCount = 0, partErrorFlag = -1; loopCount < wordLib[chWord].alignCount; loopCount++){//For each align lib
				if(wordLib[chWord].enWord[loopCount] == alignWord){//Correct Align
					if(alignWord.find(' ') != string::npos){phraseWord++;}
					else{singleWord++;}
					correctAlign++;
					wordLib[chWord].enWord[loopCount] = "";
					partErrorFlag = -2;
					fout << chWord << "," << alignWord << endl;
					break;
				}
				if(wordLib[chWord].enWord[loopCount].find(alignWord) != string::npos){
					partErrorFlag = loopCount;
				}
			}
			if(loopCount >= wordLib[chWord].alignCount && partErrorFlag != -1){//Part error align
				partErrorAlign++;
				ferr1 << chWord << "," << alignWord << "," << wordLib[chWord].enWord[partErrorFlag] << endl;
			}
			else if(partErrorFlag == -1){//Full error align
				ferr2 << tmpStr << endl;
			}
		}
	}
	//Output evalute result
	fullErrorAlign = totalAlign - correctAlign - missAlign - partErrorAlign - dropAlign;
	precisionRate = (double)correctAlign*100/(double)(totalAlign - missAlign - dropAlign);
	recallRate = (double)correctAlign*100/(double)(alignLibCount);
	fout << "==============================" << endl;
	fout << "Known Library count: " << alignLibCount << " Pairs" << endl;
	fout << "Total Chinese Align Word: " << alignCountChWord << " Pairs" << endl;
	fout << "Used Chinese Align Count: " << usedAlign << " Pairs" << endl;
	fout << "Total Align Word: " << totalAlign << " words"  << endl;
	fout << "Correct Align Word: " << correctAlign << " words(" << singleWord << "+" << phraseWord << ")"<< endl;
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
