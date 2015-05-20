#include "utilities.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>

using namespace std;
typedef map<string,string> utilityMap;

/** Common functionality which reads a file line by line and
    stores the key value pair in a map and returns the map*/
	
map<string,string> getMap(string filename){
	string line;
	ifstream myfile(filename.c_str());
	utilityMap umap;
	vector<string> words;
	if(myfile.is_open()){
		while(getline(myfile, line)){
			if(!line.empty()){
				words = getWords(line);
				umap[words.at(0)]=words.at(1);
				words.clear();
			}
		}
		myfile.close();
  	}
  	else {
    		cout<<"\n Unable to open the file: "<<filename;
	}
	return umap;
}

/** Common functionality which divides a string and 
    gets words separated by white space in a string array
    and returns the array */

vector<string> getWords(string str){
	vector<string> words;
	string word;
	stringstream os(str);
	while(os>>word){ 
		words.push_back(word);
	}
	return words;
}
