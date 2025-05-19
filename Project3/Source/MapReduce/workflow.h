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
#include "FileMgr.h"

using namespace std;

typedef MapBase<std::string, std::string>* (*CREATMAP)();
typedef ReduceBase<std::string>* (*CREATREDUCE)(std::string);

class Workflow
{
private:
	// Configurable number of map and reduce threads
	int numOfMappers;
	int numOfReducers;
	// Create a vector of files to feed into mapper threads
	std::vector<std::vector<filesystem::path>> file_partitions = std::vector<std::vector<filesystem::path>>(20);

public:
	FileMan fileObject[20];  // Create FileMan object
	// Interfaces to map and reduce librarys 
	CREATMAP createMap;
	CREATREDUCE createReduce;

	// Handles to map and reduce DLLs
	HINSTANCE hDLLMap;
	HINSTANCE hDLLReduce;
	void getMapHandler(string mapDLL);
	void getReduceHandler(string reduceDLL);
	bool runMapReduceWf(string, string, string, string, string, int, int);

	void mapProcess(MapBase<std::string, std::string>* mapClass, int m);
	void reduceProcess(ReduceBase<std::string>* reduceClass, int m);
	void reduceAggregator(ReduceBase<std::string>* reduceClass, std::vector<string>& files, string outputDirectory);
	void reduceOutputAggregator(string outputDirectory);
};

