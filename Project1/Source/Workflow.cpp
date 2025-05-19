#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <unordered_map>
#include <filesystem>

#include "Workflow.h"
#include "FileMgr.h"
#include "Map.h"
#include "Reduce.h"
#include "Sorting.h"

using std::string;

bool Workflow::runMapReduceWf(string inputDirectory, string tempDirectory, string outputDirectory) {
	FileMan fileTester;  // Create FileMan object
	Map mapTester;       // Create Map object
	Reduce reduceTester; // Create Reduce object
	Sorting sortTester; // Create Reduce object

	cout << "Starting workflow.\n" << endl;
	//Clear it if there is a file
	fileTester.clearFile(tempDirectory + "\\temp.txt");
	fileTester.clearFile(outputDirectory + "\\SUCCESS.txt");
	fileTester.clearFile(outputDirectory + "\\output.txt");

	//set the temp file up
	fileTester.setTemp(tempDirectory + "\\temp.txt");

	//initilize reduce
	reduceTester.outDir = outputDirectory ;

	if (filesystem::is_empty(inputDirectory)) {
		cout << "Input directory is empty" << endl;
		return false;
	}

	cout << "Performing the Map Reduce operation on the following input files: \n" << endl;
	//testing directories
	//run through all input files
	string holder2;
	for (const auto& entry : filesystem::directory_iterator(inputDirectory)) {
		//printing for testing
		//also do generic_string() as it is a filepath not a string
		cout << entry.path().generic_string() << endl;

		//grabs the entire thing and puts each line into a vector
		fileTester.grabFile(entry.path().generic_string());


		for (int i = 0; i < fileTester.vecSize; i++) {
			holder2 = fileTester.readVecLine(i);
			
			//testing remnant
			//cout << i << endl;

			//Needs to be before the map
			//letting our export know to send in map
			if (i == fileTester.vecSize - 1) {
				mapTester.setLastLine(true);
			}

			//I shouldnt need to check for eof since the vector will only be so big
			mapTester.mapFunc(fileTester.getTemp(), holder2);


		}
		//reset values
		mapTester.clearBufferVector();


		//after completing a file clear the vector for the next file
		fileTester.resetVecSize();
		fileTester.resetVector();

	}

	//now we sort the results and return them to reduce	
	vector<string> reduceData = sortTester.sortFile(fileTester.getTemp());
	
	for (int i = 0; i < reduceData.size(); i++) {
		reduceTester.reduceFunc(reduceData[i]);
	}
	cout << "\n\n" << endl;
	cout << "All the operations are complete" << endl;

	return true;
	
}
