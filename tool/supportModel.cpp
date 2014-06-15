/*
	Author: Paul Chen
	Date: 2014/06/07
	Target: Implement support model from paper
*/
#include "library.h"
using namespace std;

bool getBilingualIntersection(map<string, int> &ch1, map<string, int> &en1, map<string, int> &ch2, map<string, int> &en2, vector<string> &en2SegSeguence, map<string, int> &intersectionPool);
mutex gMutex;
int main(int argc, char* argv[]){
	const string KNOWN_CHWORD_PATH = "../data/knownChWord";
	const string EN_FILTER_LIB = "../data/languageBase/specialWordLibEn";
	const string CH_LAW_PATH = "../data/chBaseTrain";
	const string EN_LAW_PATH = "../data/enBaseTrain";
	//const string CH_LAW_PATH = "../data/chTest";
	//const string EN_LAW_PATH = "../data/enTest";
	const string OUTPUT_PATH = "../data/supportAlign";
	const int CANDIDATE_DEGREE = 3;
	const int MAX_THREAD = 128;
	const bool SWITCH_USE_KNOWN_LIB = false;
	map<string, int> intersectionPool; //(ch,en),freq
	map<string, int> enFilterLib;
	map<string, int>::iterator iter;
	vector<string> lawSeg;
	vector<string> knownWord;
	vector<string> chLawList;
	vector<string> enLawList;
	vector<string> chSentecneSeg;
	map<string, int> emptyMap;
	map<string, int> chWordInfo;//Record chWord and Frequency
	vector<map<string, int>> chLawSegList;
	vector<map<string, int>> enLawSegList;
	vector<vector<string>> enLawSequenceList;
	int i, j, k, listIdx, flag, threadFlag, breakFlag;
	char buf[4096];
	string tmpStr;
	fstream fin, fout;
	std::thread 	t[128];

	loadFile(EN_FILTER_LIB, enFilterLib);
	loadFile(CH_LAW_PATH, chLawList);
	loadFile(EN_LAW_PATH, enLawList);
	if(SWITCH_USE_KNOWN_LIB == true){loadFile(KNOWN_CHWORD_PATH, knownWord);}
	cerr << "\E[1;32;40mInitialize...\E[0m" << endl;
	for(i = 0, listIdx = 0, flag = 0; i < chLawList.size(); i++, flag = 0){//For each law pair
		//Chinese
		if(SWITCH_USE_KNOWN_LIB == true){
			for(j = 0; j < knownWord.size()-1; j++){
				if(chLawList[i].find(knownWord[j]) != string::npos){
					if(flag == 0){
						chLawSegList.push_back(emptyMap);
						enLawSegList.push_back(emptyMap);
					}
					chLawSegList[listIdx][knownWord[j]] = 1;
					flag++;
				}
			}
			if(flag == 0){continue;}//Have not useful chword: drop it!
		}
		else{
			explode(' ', chLawList[i], lawSeg);
			for(j = 0; j < lawSeg.size(); j++){//For each seg
				tmpStr = lawSeg[j];
				if(chWordInfo.find(tmpStr) == chWordInfo.end()){	chWordInfo[tmpStr] = 1;}
				else{chWordInfo[tmpStr]++;}
				chLawSegList.push_back(emptyMap);
				chLawSegList[listIdx][tmpStr] = 1;
			}
		}
		//English
		explode(' ', enLawList[i], lawSeg);
		for(j = 0; j < lawSeg.size(); j++){
			if(enFilterLib.find(lawSeg[j]) != enFilterLib.end()){//Have to filt out
				continue;
			}
			enLawSegList.push_back(emptyMap);
			enLawSegList[listIdx][lawSeg[j]] = 1;
		}
		enLawSequenceList.push_back(lawSeg);
		listIdx++;
		if((i+1) % 1000 == 0){
			cerr << "\t\E[1;34;40mLoad " << (i+1) << " laws\E[0m" << endl;
		}
	}
	cerr << "\t\E[1;34;40mAll Load " << (i+1) << " laws\E[0m" << endl;

	if(SWITCH_USE_KNOWN_LIB == false){
		for(iter = chWordInfo.begin(); iter != chWordInfo.end(); iter++){
			tmpStr = iter->first;
			for(i = 0, breakFlag = 0; i < tmpStr.length(); i++){
				if(tmpStr[i] >= '0' && tmpStr[i] <= '9'){breakFlag++; break;}
			}
			if(breakFlag != 0){continue;}
			knownWord.push_back(iter->first);
		}
	}
	cerr << "\t\E[1;34;40mAll Load " << knownWord.size() << " chinese words\E[0m" << endl;



	cerr << "\E[1;32;40mGet Intersection for each law pair\E[0m" << endl;
	for(i = 0, threadFlag = 0; i < listIdx; i++){
		for(j = i+1; j < listIdx; j++){
			getBilingualIntersection(chLawSegList[i], enLawSegList[i], chLawSegList[j], enLawSegList[j], enLawSequenceList[j] ,intersectionPool);
/*
			t[threadFlag++] = std::thread(&getBilingualIntersection, ref(chLawSegList[i]), ref(enLawSegList[i]), ref(chLawSegList[j]), ref(enLawSegList[j]), ref(enLawSequenceList[j]) ,ref(intersectionPool));
			if(threadFlag == MAX_THREAD){//Wait thread clear!
				for(k = 0; k < MAX_THREAD; k++){t[k].join();}
				threadFlag = 0;
			}
*/
		}
		if((i+1) % 10 == 0){
			cerr << "\E[1;35;40m" << (i+1) << "/" << listIdx << " laws is finish!!\E[0m" << endl;
		}
	}
//	for(k = 0; k < threadFlag; k++){t[k].join();}

	fout.open(OUTPUT_PATH.c_str(), ios::out);
	for(iter = intersectionPool.begin(); iter != intersectionPool.end(); iter++){
		if(iter->second < CANDIDATE_DEGREE){continue;}
		fout << iter->first << "," << iter->second << endl;
	}
	fout.close();
	
	return 0;
}

bool getBilingualIntersection(map<string, int> &ch1, map<string, int> &en1, map<string, int> &ch2, map<string, int> &en2,vector<string> &en2SegSeguence, map<string, int> &intersectionPool){
	map<string, int>::iterator iter;
	vector<string> chInterSet;
	vector<string> enInterSet;
	vector<int> enSequenceFlag;
	string tmpStr;
	int i, j, flag;
/*
	for(i = 0; i < en2SegSeguence.size(); i++){
		enSequenceFlag.push_back(0);
	}
*/
	for(iter = ch1.begin(); iter != ch1.end(); iter++){//For chinese
		tmpStr = iter->first;
		if(tmpStr.length() < 3){continue;}
		if(ch2.find(iter->first) != ch2.end()){	//Have iterator!!
			chInterSet.push_back(tmpStr);
		}
	}
	for(iter = en1.begin(); iter != en1.end(); iter++){//For english
		tmpStr = iter->first;
		if(en2.find(tmpStr) != en2.end() && tmpStr.length() > 2){	//Have iterator!!
			enInterSet.push_back(tmpStr);
		}
	}
	
	//Output result
	if((chInterSet.size() == 1 && chInterSet.size() > 0)
	&& (enInterSet.size() == 1 && enInterSet.size() > 0)){//Output intersection
		for(i = 0; i < enInterSet.size(); i++){
			tmpStr = chInterSet[0] + "," + enInterSet[i];
			if(intersectionPool.find(tmpStr) == intersectionPool.end()){
				//gMutex.lock();
				intersectionPool[tmpStr] = 1;
				//gMutex.unlock();
			}
			else{
				//gMutex.lock();
				intersectionPool[tmpStr]++;
				//gMutex.unlock();
			}
		}
	}
	return true;
}
