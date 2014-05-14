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
	const string KNOWN_ALIGN_LIB_PATH = "../data/knownWordAlign";
	const string PART_ERROR_RESULT_PATH = "../data/errorRecordPart";
	const string FULL_ERROR_RESULT_PATH = "../data/errorRecordFull";
	const double FILT_PROBABILITY = 0.1;
	const int FILT_ALIGNMENT_FREQUENCY = 1;//More than ...
	const int FILT_CONTEXT_FREQUENCY = 1;//More than ...
	const bool FILTER_SWITCH = true;//true = do filter
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
	string tmpStr, chWord, alignWord, alignProbability, alignFrequency;
	int pos1, pos2, pos3,loopCount, partErrorFlag, i, filtFlag, alignCountChWord = 0, singleWord = 0, phraseWord = 0;
	int correctAlign = 0, partErrorAlign = 0, fullErrorAlign = 0 , missAlign = 0, dropAlign = 0 ,totalAlign = 0, alignLibCount = 0, usedAlign = 0;
	double probability, frequency;
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
		alignProbability = candidateSeg[2];
		alignFrequency = candidateSeg[3];
		//-------------------Filter--------------------
		if(alignProbability.find("e") != string::npos){
			filtFlag = 1;
		}
		else if(probability = atof(alignProbability.c_str())){
			if(probability < FILT_PROBABILITY){filtFlag = 1;}
			else if(probability >= 1 && probability <= FILT_ALIGNMENT_FREQUENCY){filtFlag = 1;}
		}
		if(frequency = atof(alignFrequency.c_str())){
			if(frequency <= FILT_CONTEXT_FREQUENCY){filtFlag = 1;}
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
	fout << "==============================" << endl;
	fout << "Total Chinese Align Word: " << alignCountChWord << " Pairs" << endl;
	fout << "Used Chinese Align Count: " << usedAlign << " Pairs" << endl;
	fout << "Total Align Word: " << totalAlign << " words"  << endl;
	fout << "Correct Align Word: " << correctAlign << " words(" << singleWord << "+" << phraseWord << ")"<< endl;
	fout << "Part Error Align Word: " << partErrorAlign << " words" << endl;
	fout << "Full Error Align Word: " << fullErrorAlign << " words" << endl;
	fout << "Drop Error Align Word: " << dropAlign << " words" << endl;
	fout << "Miss Align Word: " << missAlign << " words" << endl;
	fout << "Alignment Precision Rate(correctAlign/Not Miss Align): " << (double)correctAlign*100/(double)(totalAlign - missAlign - dropAlign) << "%" << endl;
	fout << "Alignment Recall Rate(correctAlign/have be find align): " << (double)correctAlign*100/(double)(usedAlign) << "%" << endl;
	fin.close();
	fout.close();
	
	return 0;
}
