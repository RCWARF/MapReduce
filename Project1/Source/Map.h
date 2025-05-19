#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "FileMgr.h"

using namespace std;

class Map {
public:
	//create a buffer to export all the data in one go
	vector<string> bufferData;
	

	//there has to be a better way to detect end of mapping to then export the buffer but meh whatever
	bool lastLine = false;
	bool lastWord = false;

	//change the bool for the last line
	void setLastLine(bool boolVal) {
		lastLine = boolVal;
	}

	//clears the vector
	void clearBufferVector() {
		bufferData.clear();
	}

	//pass temp func and the line of words
	void mapFunc(string fileName, string temp);


	//I have no idea how else to export this data. I mean i guess you can return it to main? after that then export it
	//Doing that doesn't make sense since you would just return in the map function. This would then cause the export function to be redundent
	//it works idk
	void exportFunc(string fileName, string temp);
};

#endif
