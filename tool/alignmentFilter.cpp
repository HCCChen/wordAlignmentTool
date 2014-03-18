/*
	Author: Paul Chen
	Date: 2014/03/18
	Target: Filt the wrong align result
*/
#include "library.h"
using namespace std;

int main(int argc, char* argv[]){
	char buf[4096];
	string tmpStr, chWord, alignWord, alignScore;
	const string OUTPUT_PATH = "../data/alignResult";
	vector<string> filePath;
	fstream fin, fout;
	map<string, int> alignResult;
	map<string, int>::iterator iter;
	int i, pos1, pos2;

	regex_t regexWord, regexAlign;
	int reti, reti2;
	reti = regcomp(&regexWord, "[a-zA-Z0-9]", 0);
	reti2 = regcomp(&regexAlign, "[0-9(){},.;:]", 0);
	//Get file Path from parameter
	if(argc > 1){
		for(i = 1; i < argc; i++){
			tmpStr.assign(argv[i]);
			filePath.push_back(tmpStr);
		}
	}
	else{
		cout << "Please Input File Name!!" << endl;
		return 0;
	}
	//Load Alignment Result and record it!

	for(i = 0; i < filePath.size(); i++){
		cout << filePath[i] << endl;
		fin.open(filePath[i].c_str(), ios::in);
		while(!fin.eof()){
			fin.getline(buf, 4096);
			tmpStr.assign(buf);
			pos1 = tmpStr.find(',');
			pos2 = tmpStr.find_last_of(',');
			//Divide Information
			chWord = tmpStr.substr(0, pos1);
			alignWord = tmpStr.substr(pos1+1, pos2 - pos1 - 1);
			alignScore = tmpStr.substr(pos2 + 1);
			//Filter
			reti = regexec(&regexWord, chWord.c_str(), 0, NULL, 0);
			reti2 = regexec(&regexAlign, alignWord.c_str(), 0, NULL, 0);
			if( !reti || !reti2 || chWord.length() < 3 || alignWord.length() < 3 || alignScore.find('e') != string::npos){
				continue;
			}
			else if(alignResult.find(tmpStr) == alignResult.end()){
				alignResult[tmpStr] = 1;
			}
		}
		fin.close();
	}
	fout.open(OUTPUT_PATH.c_str(), ios::out);
	for(iter = alignResult.begin(); iter != alignResult.end(); iter++){
		fout << iter->first << endl;
	}
	fout.close();

	return 0;
}
