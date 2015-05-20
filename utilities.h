/** Header file for utilities.cpp*/

#ifndef utilities_h
#define utilities_h
#include <string>
#include <map>
#include <vector>

using namespace std;
typedef map<string,string> utilityMap;
 
map<string,string> getMap(string filename);
vector<string> getWords(string str);
 
#endif
