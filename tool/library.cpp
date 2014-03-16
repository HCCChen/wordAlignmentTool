#include "library.h"

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


string int2str(int &i) {
	string s;
	stringstream ss(s);
	ss << i;
	return ss.str();
}

int str2int(string str){
	return atoi(str.c_str());
}
