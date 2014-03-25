/*
	Author: Paul Chen
	Date: 2014/02/27
	Target: 切分法規資料，分成中/英文版本
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <regex.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "library.h"
using namespace std;

fstream foutCh, foutEn;

//讀檔並切分檔案
bool divideLaw(string filePath);
//過濾干擾字串
string filtExtraString(string source, string flag1, string flag2);

int main(int argc, char* argv[]){
	const string DIR_PATH = "../data/";
	const string DIR_NAME = "BiLawSentence/";
	vector<string> files = vector<string>();
	getdir(DIR_PATH + DIR_NAME, files);
	foutCh.open("../data/chBase", ios::out);
	foutEn.open("../data/enBase", ios::out);
    //輸出資料夾和檔案名稱於螢幕
    for(int i=0; i<files.size(); i++){
		if(divideLaw(DIR_PATH + DIR_NAME + files[i]) == false){
			cout << files[i] << " has error, ignore!!" << endl;
		}
    }
	return 0;
}


string filtExtraString(string source, char flag1, char flag2){
	int pos1, pos2;
	while(source.find(flag1) != string::npos){
		pos1 = source.find(flag1);
		pos2 = source.find(flag2);
		source.replace(pos1, pos2-pos1+1, "");
	}
	return source;
}

bool divideLaw(string filePath){
	fstream fin;
	int pairFlag = 0;
	char buf[4096];
	string lawSentence, chBuf, enBuf;
	vector<string> chStack = vector<string>();
	vector<string> enStack = vector<string>();
	regex_t regexComment;
	regex_t regexLanguage;
	int reti, reti2;
	char msgbuf[60000];
	/* Compile regular expression */
	reti = regcomp(&regexComment, "[\[]", 0);
	reti2 = regcomp(&regexLanguage, "[a-zA-Z]", 0);
	
	fin.open(filePath.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		lawSentence.assign(buf);
		if(lawSentence.length() < 1){break;}
		//Divide Info
		switch(pairFlag){
			case 0:
				pairFlag = 1;
				chBuf = lawSentence;
			break;
			case 1://判斷是否為合法配對
				pairFlag = 0;
				enBuf = lawSentence;

				if((chBuf.find('[') < 2 || chBuf.length() < 4) && (enBuf.find('[') < 2 || enBuf.length() < 4)){//End Of File
					break;
				}
				//過濾註解符號
				chBuf = filtExtraString(chBuf, '[', ']');
				enBuf = filtExtraString(enBuf, '[', ']');

				reti2 = regexec(&regexLanguage, chBuf.c_str(), 0, NULL, 0);
				if( !reti2 ){//中文文句包含英文
					return false;
				}
				//合法對句
				chStack.push_back(chBuf);
				enStack.push_back(enBuf);
				
			break;
		}
	}
	fin.close();
	for(vector<string>::iterator it = chStack.begin(); it != chStack.end();it++) {  
    	foutCh << *it << endl;
	}
	for(vector<string>::iterator it = enStack.begin(); it != enStack.end();it++) {  
    	foutEn << *it << endl;
	}
	return true;
}
