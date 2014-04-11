/*
	Author: Paul Chen
	Date: 2014/03/11
	Target: Maege phrase by 'prep.' flag, base on "gizaDecodeResult"
*/
#include "library.h"

typedef struct alignLib{
	map<string, int>alignWordLib;	//For basic Align Word(Map)

	string mergeWord[128];	//For post-process align Word
	int mergeCount;

}ALIGNPOOL;

//Merge Phrase by some Rule!!!
string mergePhrase(vector<string> wordSeg, vector<int> wordFlag, string basicWord);

int main(int argc, char* argv[]){
	fstream fin, fin2, fout;
	const string ALIGN_WORD_LIB = "../data/gizaDecodeResult";
	const string CH_LAW_PATH = "../data/chBaseTrain";
	const string EN_LAW_PATH = "../data/enBaseTrain";
	const string OUTPUT_PATH = "../data/phraseResult";
	char buf[65536];
	string tmpStr, baseWord, alignWord, mergeResult;
	string chLaw, enLaw;
	map<string, ALIGNPOOL> alignLib;	//Map of structure of map
	vector<string> chWordSeg;
	vector<string> enWordSeg;
	vector<int> enWordFlag;
	int reti, pos1, pos2;
	int i,j,flag;	//Functional Variable
	regex_t regexRef;
	reti = regcomp(&regexRef, "[0-9a-zA-Z]", 0);

	fin.open(ALIGN_WORD_LIB.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 65536);
		tmpStr.assign(buf);
		pos1 = tmpStr.find(',');
		pos2 = tmpStr.find_last_of(',');
		baseWord = tmpStr.substr(0, pos1);
		alignWord = tmpStr.substr(pos1 + 1, pos2 - pos1 - 1);
		reti = regexec(&regexRef, baseWord.c_str(), 0, NULL, 0);
		if( !reti ){//中文文句包含英文數字
			continue;
		}
		//Put align word into alignLib
		alignLib[baseWord].alignWordLib[alignWord] = 1;
	}
	fin.close();


	//Merge Phrase
	fin.open(CH_LAW_PATH.c_str(), ios::in);
	fin2.open(EN_LAW_PATH.c_str(), ios::in);
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	while(!fin.eof() && !fin2.eof()){//For each Pair
		//Load Law Sentence info
		fin.getline(buf, 65536);
		chLaw.assign(buf);
		explode(' ', chLaw, chWordSeg);
		fin2.getline(buf, 65536);
		enLaw.assign(buf);
		explode(' ', enLaw, enWordSeg);
		enWordFlag.clear();
		enWordFlag.resize(enWordSeg.size(), 0);
		//Check Chinese Word had been align or not
		for(i = 0; i < chWordSeg.size(); i++){//For each "Chinese Word" ==> for each Align result
			if(chWordSeg[i].length() < 6){continue;}
			if(alignLib.find(chWordSeg[i]) != alignLib.end()){//If Chinese is exist
				//初始化
				enWordFlag.clear();
				enWordFlag.resize(enWordSeg.size(), 0);
				for(j = 0, flag = 0; j < enWordSeg.size(); j++){//for each english word
					if( alignLib[chWordSeg[i]].alignWordLib[enWordSeg[j]] == 1){
						enWordFlag[j] = 1;
						flag++;
					}
				}
				if(flag != 0){
					mergeResult = mergePhrase(enWordSeg, enWordFlag, chWordSeg[i]);
					if(mergeResult.length() >= 3){
						fout << chWordSeg[i] << "," << mergeResult << ",0" << endl;
					}
				}
			}
		}
	}
	fin.close();
	fin2.close();
	fout.close();

/*Show align Lib
	map<string, ALIGNPOOL>::iterator iter;
	for(iter = alignLib.begin(); iter != alignLib.end(); iter++){
		cout << iter->first << endl;
		for(int i = 0; i < iter->second.enWordCount; i++){
			cout << "\t" << iter->second.alignWord[i] << endl;
		}
	}
*/

	
	return 0;
}

string mergePhrase(vector<string> wordSeg, vector<int> wordFlag, string baseWord){//For each word seg
	string tmpStr = "", buf;
	int i, flag, posHead, posTail, tmpPos1, tmpPos2;
	string *prepLib = new string[21]{"at", "on", "in", "to", "for", "of", "from", "by", "such", "who", "what", "where", "when", "why", "how", "is", "are", "am", "a", "an", "the"};
	//Basic conception: find longest continue '1'
	if(wordSeg.size() > 0){
		for(i = 0, tmpPos1 = -1, tmpPos2 = -1; i < wordSeg.size(); i++){//For each segment
			if(wordFlag[i] == 1){//Find it
				if(tmpPos1 == -1){//New continue sequence
					tmpPos1 = i;
				}
				else{
					tmpPos2 = i;
				}
			}
			else{//Break sequence
				if((posTail - posHead + 1) <= (tmpPos2 - tmpPos1 + 1) && tmpPos2 != -1){
					posHead = tmpPos1;
					posTail = tmpPos2;
					tmpPos1 = -1;
					tmpPos2 = -1;
				}
			}
		}
		//Post process and Output result
		if(posHead != -1 && posTail - posHead > 1 && posTail - posHead <= 1 + baseWord.length()/3){
			//Post Process
			for(i = 0; i < 21; i++){//Remove prep word
				if(wordSeg[posHead] == prepLib[i]){
					posHead++;
				}
				if(wordSeg[posTail] == prepLib[i]){
					posTail--;
				}
			}
			//Output
			for(i = posHead; i <= posTail; i++){
				//tmpStr += wordSeg[i] + "(" + int2str(i) + ") ";
				tmpStr += wordSeg[i] + " ";
			}
		}
	}
	else{
		cout << "EMPTY!!" << endl;
	}
	tmpStr = tmpStr.substr(0, tmpStr.length()-1);
	return tmpStr;
}

