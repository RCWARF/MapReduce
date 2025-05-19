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

typedef MapBase<std::string, std::string>* (*CREAT_MAP)();
typedef ReduceBase<std::string>* (*CREAT_RED)(std::string);

bool Workflow::runMapReduceWf(string inputDirectory, string tempDirectory, string outputDirectory, string mapDLL, string reduceDLL) {
	FileMan fileTester;  // Create FileMan object
	Sorting sortTester; // Create Reduce object
	

	BOOST_LOG_TRIVIAL(info) << "Starting workflow.";
	cout << inputDirectory << endl << tempDirectory << endl << outputDirectory << endl << mapDLL << reduceDLL << endl;

	//Clear it if there is a file
	fileTester.clearFile(tempDirectory + "\\temp.txt");
	fileTester.clearFile(outputDirectory + "\\SUCCESS.txt");
	fileTester.clearFile(outputDirectory + "\\output.txt");

	//set the temp file up
	fileTester.setTemp(tempDirectory + "\\temp.txt");

	std::wstring widestrMap = std::wstring(mapDLL.begin(), mapDLL.end());
	const wchar_t* widecstrMap = widestrMap.c_str();
	// Create a handle to map DLL
	hDLLMap = LoadLibraryEx(widecstrMap, NULL, NULL);   // Handle to map DLL
	if (!hDLLMap) {
		BOOST_LOG_TRIVIAL(fatal) << "Info in Workflow constructor: Map DLL Error.";
	}

	std::wstring widestrReduce = std::wstring(reduceDLL.begin(), reduceDLL.end());
	const wchar_t* widecstrReduce = widestrReduce.c_str();
	hDLLReduce = LoadLibraryEx(widecstrReduce, NULL, NULL);   // Handle to Reduce DLL
	if (!hDLLReduce) {
		BOOST_LOG_TRIVIAL(fatal) << "Info in Workflow constructor: Reduce DLL Error.";
	}

	createMap = (CREAT_MAP)GetProcAddress(hDLLMap, "createMapper");
	// If function pointer to createMap fails to be created, log and exit
	if (createMap == NULL)
	{
		BOOST_LOG_TRIVIAL(fatal) << "Fatal in Workflow constructor: Function pointer to createMap is NULL.";
		exit(-1);
	}

	createReduce = (CREAT_RED)GetProcAddress(hDLLReduce, "createReducer");
	// If create_reduce_ function pointer is NULL, then log and exit
	if (createReduce == NULL)
	{
		BOOST_LOG_TRIVIAL(fatal) << "Fatal in Workflow constructor: Function pointer to create_reduce_ is NULL.";
		exit(-1);
	}

	MapBase<std::string, std::string>* mapTester = createMap();
	ReduceBase<std::string>* reduceTester = createReduce(outputDirectory);
	

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
				mapTester->setLastLine(true);
			}

			//I shouldnt need to check for eof since the vector will only be so big
			mapTester->mapFunc(fileTester.getTemp(), holder2);


		}
		//reset values
		mapTester->clearBufferVector();


		//after completing a file clear the vector for the next file
		fileTester.resetVecSize();
		fileTester.resetVector();

	}

	//now we sort the results and return them to reduce	
	vector<string> reduceData = sortTester.sortFile(fileTester.getTemp());

	for (int i = 0; i < reduceData.size(); i++) {
		reduceTester->reduceFunc(reduceData[i]);
	}
	cout << "\n\n" << endl;
	cout << "All the operations are complete" << endl;

	return true;

}
