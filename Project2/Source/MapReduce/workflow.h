#pragma once
#

#include <string>
#include <iostream>
#include <sys/stat.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <filesystem>
#include <boost/log/trivial.hpp>
#include "Map.h"
#include "Reduce.h"

using namespace std;

typedef MapBase<std::string, std::string>* (*CREATMAP)();
typedef ReduceBase<std::string>* (*CREATREDUCE)(std::string);

class Workflow
{

public:
	// Interfaces to map and reduce librarys 
	CREATMAP createMap;
	CREATREDUCE createReduce;

	// Handles to map and reduce DLLs
	HINSTANCE hDLLMap;
	HINSTANCE hDLLReduce;
	bool runMapReduceWf(string, string, string, string, string);

};

