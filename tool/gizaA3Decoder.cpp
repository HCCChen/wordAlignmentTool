/*
	Author: Paul Chen
	Date: 2014/3/16
	Target: Decode Giza++ A3 result
*/
#include "library.h"

typedef struct alignLib{
	map<string, int>mergeWord;	//For basic Align Word(Map)
}ALIGNPOOL;

bool decodeA3Result(string chSentence, string enSentence, map<string, ALIGNPOOL> &alignResult);
void printResult(pair<string, ALIGNPOOL> p);
fstream fout;

int main(int argc, char* argv[]){
	const string GIZA_RESULT_PATH = "../giza++/c2e.A3.final";
	const string OUTPUT_PATH = "../data/gizaA3DecodeResult";
	fstream fin;
	map<string, ALIGNPOOL> alignResult;
	int i,j;//loop Counter
	int pos1, pos2, flag;
	string chSentence, enSentence;
	char buf[4096];

	fin.open(GIZA_RESULT_PATH, ios::in);
	fout.open(OUTPUT_PATH, ios::out);
	flag = 0;
	while(!fin.eof()){
		fin.getline(buf, 4096);
		switch(flag){
			case 0:
				flag++;
			break;
			case 1:
				enSentence.assign(buf);
				flag++;
			break;
			case 2:
				chSentence.assign(buf);
				flag = 0;
				decodeA3Result(chSentence, enSentence, alignResult);
				//Do Decoder
			break;
		}
	}
	
	for_each(alignResult.begin(), alignResult.end(), printResult);

	fin.close();
	fout.close();

	return 0;
}

bool decodeA3Result(string chSentence, string enSentence, map<string, ALIGNPOOL> &alignResult){
	vector<string> chSeg, enSeg, alignIdx;
	string chWord = "", chAlignIdx, alignMergeBuf;
	int i,j, pos1, pos2, preIdx;
	//Divide English Word
	enSentence = "NULL " + enSentence;
	explode(' ', enSentence , enSeg);
	//Divide Chinese Word and alignment number
	explode(')', chSentence , chSeg);	//For each word and align result
	for(i = 1; i < chSeg.size(); i++){
		pos1 = chSeg[i].find('(');
		chWord = chSeg[i].substr(1, pos1-2);
		pos1 = chSeg[i].find('{');
		pos2 = chSeg[i].find('}');
		chAlignIdx = chSeg[i].substr(pos1+2, pos2-pos1-2);
		if(chAlignIdx.length() < 2){continue;}
		explode(' ', chAlignIdx, alignIdx);
		for(j = 0, preIdx = -2, alignMergeBuf = ""; j < alignIdx.size()-1; j++){//For each align idx
			//cout << chWord << ','<< enSeg[str2int(alignIdx[j])] << endl;
			if(preIdx + 1 != str2int(alignIdx[j])){//Not continue index sequence or a head of new sequence
				if(alignMergeBuf != ""){//Merge Finish, Save it!
					if(alignResult[chWord].mergeWord.find(alignMergeBuf) == alignResult[chWord].mergeWord.end()){//New word
						alignResult[chWord].mergeWord[alignMergeBuf] = 1;
					}
					else{
						alignResult[chWord].mergeWord[alignMergeBuf]++;
					}
				}
				//New phrase begin
				alignMergeBuf = enSeg[str2int(alignIdx[j])];
			}
			else{//Merge phrase
				alignMergeBuf += " " + enSeg[str2int(alignIdx[j])];
			}
			preIdx = str2int(alignIdx[j]);
		}
	}
	//Convert it !
}

void printResult(pair<string, ALIGNPOOL> p){
	string basicWord = p.first;
	map<string, int> mergeLib = p.second.mergeWord;
	map<string, int>::iterator iter;
	for(iter = mergeLib.begin(); iter != mergeLib.end(); iter++){
//		if(iter->second > 2){
			fout << p.first << ',' << iter->first << "," << iter->second << endl;
//		}
	}

}
