/*
	Author: Paul Chen
	Date: 2014//
	Target: 
*/
#include "library.h"

int getdir(string dir, vector<string> &files){
	DIR *dp;//創立資料夾指標
	struct dirent *dirp;
	if((dp = opendir(dir.c_str())) == NULL){
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}
	while((dirp = readdir(dp)) != NULL){//如果dirent指標非空
		files.push_back(string(dirp->d_name));//將資料夾和檔案名放入vector
	}
	closedir(dp);//關閉資料夾指標
	return 0;
}

int countFileLine(string filePath){
	ifstream	fin;		//file point
	int 		count = 0;
	char 		ch[65536];		//Buffer
	fin.open(filePath.c_str());
	while(fin && !fin.eof()) { 
		fin.getline(ch, 65536);
		count++;
	}
	return count;
}

int explode(char divideChar, string originalString, vector<string> &stringAry){
	int flag = 0, preFlag = 0 ,index = 0;
	int length = originalString.length();
	string tmpStr;
	stringAry.clear();
	while(flag = originalString.find(divideChar, flag)){
		tmpStr = originalString.substr(preFlag, flag-preFlag);
		stringAry.push_back(tmpStr);
		index++;
		if(flag > length || flag < 0){return index;}
		flag++;
		preFlag = flag;
	}
	return -1;
}

int explode(string divideStr, string originalString, vector<string> &stringAry){
	int flag = 0, preFlag = 0 ,index = 0;
	int length = originalString.length();
	string tmpStr;
	stringAry.clear();
	while(flag = originalString.find(divideStr, flag)){
		tmpStr = originalString.substr(preFlag, flag-preFlag);
		stringAry.push_back(tmpStr);
		index++;
		if(flag > length || flag < 0){return index;}
		flag += divideStr.length();
		preFlag = flag;
	}
	return -1;
}


string int2str(int &i) {
	string s;
	stringstream ss(s);
	ss << i;
	return ss.str();
}

string double2str(double &i){
	string s;
	stringstream ss(s);
	ss << i;
	return ss.str();
}

int str2int(string str){
	return atoi(str.c_str());
}

string strReplaceAll(string sources, string originStr, string targetStr){
	string tmpStr1, tmpStr2;
	int i, j, pos;
	while(sources.find(originStr) != string::npos){
		pos = sources.find(originStr);
		tmpStr1 = sources.substr(0, pos);
		tmpStr2 = sources.substr(pos+originStr.length());
		sources = tmpStr1 + targetStr + tmpStr2;
	}
	return sources;
}

string mergeSequence(string mergeSeq, vector<string> wordSeq){
	int i;
	string result = "";
	for(i = 0; i < wordSeq.size() ;i++){
		result += wordSeq[i] + mergeSeq;
	}
	return result;
}

string strToLower(string str){
	int i;
	for(i = 0; i < str.length(); i++){
		if(str[i] >= 'A' && str[i] <= 'Z'){//To lower
			str[i] += 'a' - 'A';
		}
	}
	return str;
}

bool loadFile(string filename, vector<string> &container){
	fstream fin;
	char buf[4096];
	string tmpStr;
	fin.open(filename.c_str(), ios::in);
	if(fin.eof()){return false;}
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.length() < 1){continue;}
		container.push_back(tmpStr);
	}
	fin.close();
	return true;
}

bool loadFile(string filename, map<string, int> &container){
	fstream fin;
	char buf[4096];
	string tmpStr;
	fin.open(filename.c_str(), ios::in);
	if(fin.eof()){return false;}
	while(!fin.eof()){
		fin.getline(buf, 4096);
		tmpStr.assign(buf);
		if(tmpStr.length() < 1){continue;}
		container[tmpStr] = 0;
	}
	fin.close();
	return true;
}

double getMutualInformation(int totalNumber, int partA, int partB, int partAB){
	double p_ab, p_a, p_b, mu;
	p_ab = (double)partAB/(double)totalNumber;
	p_a = (double)(partA+partAB)/(double)totalNumber;
	p_b = (double)(partB+partAB)/(double)totalNumber;
	mu = log(p_ab/(p_a*p_b));
	//cout << mu << "=" << p_ab << "/" << p_a << "*" << p_b << endl;
	return mu;
}
