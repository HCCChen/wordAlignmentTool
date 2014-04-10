/*
	Author: Paul Chen
	Date: 2014/3/4
	Target: Evaluate Word Alignment result
*/
#include <iostream>
#include <fstream>
#include <string>
#include <map>
using namespace std;

typedef struct ALIGNLIB{
	string enWord[32];
	int alignCount;
}alignLib;

int main(int argc, char* argv[]){
	const string KNOWN_ALIGN_LIB_PATH = "../data/knownWordAlign";
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
	fstream fin, fout;
	char buf[4096];
	string tmpStr, chWord, alignWord;
	int pos1, pos2, loopCount, partErrorFlag;
	int correctAlign = 0, partErrorAlign = 0 , missAlign = 0, totalAlign = 0, alignLibCount = 0;
	//load align lib
	fin.open(KNOWN_ALIGN_LIB_PATH.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.length() < 3){continue;}
		alignLibCount++;
		chWord = tmpStr.substr(0, tmpStr.find(','));
		alignWord = tmpStr.substr(tmpStr.find(',')+1);
		//Put in the word lib
		if(wordLib.find(chWord) == wordLib.end()){//New word
			wordLib[chWord].enWord[0] = alignWord;
			wordLib[chWord].alignCount = 1;
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
	while(!fin.eof()){//For each Align Result
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		//Divide Word
		totalAlign++;
		pos1 = tmpStr.find(',');
		pos2 = tmpStr.find_last_of(',');
		chWord = tmpStr.substr(0, pos1);
		alignWord = tmpStr.substr(pos1+1, pos2 - pos1 - 1);
		if(alignWord.length() < 3){continue;}
		//Judge is Align correct.
		if(wordLib.find(chWord) == wordLib.end()){//Miss align
			missAlign++;
		}
		else{//Has Align
			for(loopCount = 0, partErrorFlag = -1; loopCount < wordLib[chWord].alignCount; loopCount++){//For each align lib
				if(wordLib[chWord].enWord[loopCount] == alignWord){//Correct Align
					correctAlign++;
					wordLib[chWord].enWord[loopCount] = "";
					fout << chWord << "," << alignWord << endl;
					break;
				}
				if(wordLib[chWord].enWord[loopCount].find(alignWord) != string::npos){
					partErrorFlag = loopCount;
				}
			}
			if(loopCount >= wordLib[chWord].alignCount && partErrorFlag != -1){
				partErrorAlign++;
				fout << "\t" << chWord << "," << alignWord << "," << wordLib[chWord].enWord[partErrorFlag] << endl;
			}
		}
	}
	//Output evalute result
	fout << "==============================" << endl;
	fout << "Align Lib Count: " << alignLibCount << " Pairs" << endl;
	fout << "Total Align Word: " << totalAlign << " words" << endl;
	fout << "Correct Align Word: " << correctAlign << " words" << endl;
	fout << "Part Error Align Word: " << partErrorAlign << " words" << endl;
	fout << "Full Error Align Word: " << totalAlign - correctAlign - missAlign - partErrorAlign << " words" << endl;
	fout << "Miss Align Word: " << missAlign << " words" << endl;
	fout << "Alignment Correct Rate(correctAlign/Not Miss Align): " << (double)correctAlign*100/(double)(totalAlign-missAlign) << "%" << endl;
	fin.close();
	fout.close();
	
	return 0;
}
