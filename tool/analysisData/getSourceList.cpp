/*
	Author: Paul Chen
	Date: 2014/06/03
	Target: Get knownWordAlign Sources law
*/
#include "../library.h"
using namespace std;

int main(int argc, char* argv[]){
	const string KNOWN_ALIGN_PATH = "../../data/knownWordAlign20140522";
	const string LAW_SOURCES_PATH = "../../data/BiLawSentence/";
	char buf[4096];
	int pos1, i,j ,loadFlag, findFlag;
	string tmpStr, chLaw, enLaw;
	vector<string> files = vector<string>();	//Save each File Name
	vector<string> chLib;
	vector<string> enLib;
	fstream fin, fout;
	int reti, reti2;
	regex_t regexComment;
	regex_t regexLanguage;
	/* Compile regular expression */
	reti = regcomp(&regexComment, "[\[]", 0);
	reti2 = regcomp(&regexLanguage, "[a-zA-Z]", 0);

	getdir(LAW_SOURCES_PATH, files);
	//Load KnownAlign File
	fin.open(KNOWN_ALIGN_PATH.c_str(), ios::in);
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		pos1 = tmpStr.find(",");
		chLib.push_back(trim(tmpStr.substr(0, pos1)));
		enLib.push_back(trim(tmpStr.substr(pos1 + 1)));
	}
	fin.close();
	//Load BiLaw
	for(i = 0; i < files.size(); i++){
		tmpStr = LAW_SOURCES_PATH + files[i];
		fin.open(tmpStr.c_str(), ios::in);
		loadFlag = 0;
		findFlag = 0;
		while(!fin.eof()){
			fin.getline(buf, 4096);
			tmpStr.assign(buf);
			if(tmpStr.length() < 1){break;}
			//Load info
			switch(loadFlag){
				case 0:
					loadFlag = 1;
					chLaw = tmpStr;
				break;
				case 1:
					loadFlag = 0;
					enLaw = tmpStr;
					reti2 = regexec(&regexLanguage, chLaw.c_str(), 0, NULL, 0);
					if( !reti2 ){//中文文句包含英文
						break;
					}
					//Check sources
					for(j = 0, findFlag = 0; j < chLib.size()-1, findFlag == 0, chLib[j].length() > 1; j++){
						if(chLaw.find(chLib[j]) != string::npos
						&& enLaw.find(enLib[j]) != string::npos){
							//cout << j << " : " << chLib[j] << " : " << chLaw << endl;
							//cout << j << " : " << enLib[j] << " : " << enLaw << endl;
							findFlag = 1;
						}
					}
				break;
			}					
			if(findFlag == 1){
				cout << LAW_SOURCES_PATH << files[i] << endl;
				break;
			}

		}
		fin.close();
	}
	return 0;
	//Output result
}
