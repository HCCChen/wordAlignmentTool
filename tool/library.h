/*
	Author: Paul Chen
	Date: 2014/03/16
	Target: Useful Function implement
*/
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <regex.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

using namespace std;

//integer to string
string int2str(int &i);
//integer to string
int str2int(string str);
//Divide Sentence base on flag character
int explode(char divideChar, string originalString, vector<string> &stringAry);
