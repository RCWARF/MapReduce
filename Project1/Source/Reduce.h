#ifndef REDUCE_H
#define REDUCE_H

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include "FileMgr.h"

class Reduce {
public:
	//this is needed as how else would the export function know where to export. 
	string outDir;

	//Hashing will be the fastest way to completed this
	void reduceFunc(string keyString);

	void exportFunc(string key, size_t numResult);
};

#endif
