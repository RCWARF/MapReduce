#include <stdio.h>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include <thread>

#include "Workflow.h"
#include "FileMgr.h"
#include "Map.h"
#include "Reduce.h"
#include "Sorting.h"

using namespace std;

typedef MapBase<std::string, std::string>* (*CREAT_MAP)();
typedef ReduceBase<std::string>* (*CREAT_RED)(std::string);

void Workflow::getMapHandler(string mapDLL) {
	std::wstring widestrMap = std::wstring(mapDLL.begin(), mapDLL.end());
	const wchar_t* widecstrMap = widestrMap.c_str();
	// Create a handle to map DLL
	hDLLMap = LoadLibraryEx(widecstrMap, NULL, NULL);   // Handle to map DLL
	if (!hDLLMap) {
		BOOST_LOG_TRIVIAL(fatal) << "Info in Workflow constructor: Map DLL Error.";
	}

	createMap = (CREAT_MAP)GetProcAddress(hDLLMap, "createMapper");
	// If function pointer to createMap fails to be created, log and exit
	if (createMap == NULL)
	{
		BOOST_LOG_TRIVIAL(fatal) << "Fatal in Workflow constructor: Function pointer to createMap is NULL.";
		exit(-1);
	}
}

void Workflow::getReduceHandler(string reduceDLL) {
	std::wstring widestrReduce = std::wstring(reduceDLL.begin(), reduceDLL.end());
	const wchar_t* widecstrReduce = widestrReduce.c_str();
	hDLLReduce = LoadLibraryEx(widecstrReduce, NULL, NULL);   // Handle to Reduce DLL
	if (!hDLLReduce) {
		BOOST_LOG_TRIVIAL(fatal) << "Info in Workflow constructor: Reduce DLL Error.";
	}

	createReduce = (CREAT_RED)GetProcAddress(hDLLReduce, "createReducer");
	// If create_reduce_ function pointer is NULL, then log and exit
	if (createReduce == NULL)
	{
		BOOST_LOG_TRIVIAL(fatal) << "Fatal in Workflow constructor: Function pointer to create_reduce_ is NULL.";
		exit(-1);
	}
}

bool Workflow::runMapReduceWf(string inputDirectory, string tempDirectory, string outputDirectory, string mapDLL, string reduceDLL, int mappers, int reducers) {

	FileMan* fileTester;
	BOOST_LOG_TRIVIAL(info) << "Starting workflow.";
	BOOST_LOG_TRIVIAL(info) << inputDirectory << endl << tempDirectory << endl << outputDirectory << endl << mapDLL << endl << reduceDLL << endl;


	//early declaration is needed or else we get a divide by zero error
	numOfMappers = mappers;
	numOfReducers = reducers;
	
	// Get Map handler from DLL
	getMapHandler(mapDLL);

	// Get Reduce handler from DLL
	getReduceHandler(reduceDLL);

	// Create map instance per thread
	std::vector<MapBase<std::string, std::string>*> mapTester;
	for (int i = 0; i < numOfMappers; i++) {
		mapTester.push_back(createMap());
	}

	// Create map instance per thread
	std::vector <ReduceBase<std::string>*> reduceTester;
	for (int i = 0; i < numOfMappers; i++) {
		reduceTester.push_back(createReduce(outputDirectory));
	}

	// Create a vector of files to feed into mapper threads
	//std::vector<std::vector<filesystem::path>> file_partitions (numOfMappers);

	int numberOfFiles = 0;
	for (const auto& entry : filesystem::directory_iterator(inputDirectory)) {
		numberOfFiles++;
	}

	int numberOfFilesInEachPartition = numberOfFiles / numOfMappers;
	
	numOfMappers = (mappers > numberOfFiles)? numberOfFiles: mappers; // Not possible to run more mappers than input files
	numOfReducers = (reducers > numberOfFiles) ? numberOfFiles : reducers; // Not possible to run more reduces than input files
	
	BOOST_LOG_TRIVIAL(info) << "Number of mappers: " << numOfMappers << "Number of reducers: " << numOfReducers;

	int count = 0;
	int partition = 0;
	fileTester = &fileObject[partition];
	//Clear it if there is a file
	fileTester->clearFile(tempDirectory + "\\TEMP_" + to_string(partition));
	//fileTester->clearFile(outputDirectory + "\\SUCCESS_" + to_string(partition));
	fileTester->clearFile(outputDirectory + "\\OUTPUT_" + to_string(partition));
	//set the temp file up
	fileTester->setTemp(tempDirectory + "\\TEMP_" + to_string(partition));
	fileTester->setOut(outputDirectory + "\\OUTPUT_" + to_string(partition));
	for (const auto& entry : filesystem::directory_iterator(inputDirectory)) {
		file_partitions[partition].push_back(entry);
		count++;
		if (count == numberOfFilesInEachPartition) {
			if (partition < (numOfMappers - 1)) {
				// Increment partition
				partition++;
				fileTester = &fileObject[partition];
				fileTester->clearFile(tempDirectory + "\\TEMP_" + to_string(partition));
				//fileTester->clearFile(outputDirectory + "\\SUCCESS_" + to_string(partition));
				fileTester->clearFile(outputDirectory + "\\OUTPUT_" + to_string(partition));
				//set the temp file up
				fileTester->setTemp(tempDirectory + "\\TEMP_" + to_string(partition));
				fileTester->setOut(outputDirectory + "\\OUTPUT_" + to_string(partition));
				count = 0;
			}
		}
	}

	cout << "Performing the Map Reduce operation on the following input files: \n" << endl;
	//TODO: Create intermediate files from each mapper thread
	//run through all input files
	std::vector<std::thread> mapThreads;
	for (int m = 0; m < numOfMappers; m++) {
		BOOST_LOG_TRIVIAL(info) << "Running Map function for partition: " << m;
		mapThreads.push_back(std::thread(&Workflow::mapProcess, this, mapTester[m], m));

		cout << "\n" << endl;
	}

	// Wait for all the Map threads to finish
	for (auto t = mapThreads.begin(); t != mapThreads.end(); t++) {
		t->join();
	}

	BOOST_LOG_TRIVIAL(info) << "All map processes completed successfully";
	cout << "\n" << endl;

	std::vector<std::thread> reduceThreads;
	for (int m = 0; m < numOfReducers; m++) {
		BOOST_LOG_TRIVIAL(info) << "Running Reduce function for partition: " << m;
		//reduceTester->reduceFunc(reduceData[i]);
		// Create the reduce threads ( Currently only one so runs sequentially)			
		reduceThreads.push_back(std::thread(&Workflow::reduceProcess, this, reduceTester[m], m));
	}

	// Wait for all the Reduce threads to finish
	for (auto t = reduceThreads.begin(); t != reduceThreads.end(); t++) {
		t->join();
	}

	BOOST_LOG_TRIVIAL(info) << "All Reduce processes completed successfully";
	cout << "\n" << endl;

	// g.	Perform a final reduction of all results.
	// Gather all the intermediate reduce files
	std::vector<string> reducer_output;

	for (int m = 0; m < numOfMappers; m++) {
		fileTester = &fileObject[m];
		reducer_output.push_back(fileTester->getOut());
	}

	ReduceBase<std::string>* reduceTester_1 = createReduce(outputDirectory);

	reduceAggregator(reduceTester_1, reducer_output, outputDirectory); // Incomplete
	cout << "\n\n" << endl;
	cout << "All the operations are complete" << endl;

	// Write the success file
	BOOST_LOG_TRIVIAL(info) << "Writing success file...";
	boost::filesystem::path success_path(outputDirectory + "\\SUCCESS.txt");
	boost::filesystem::ofstream success_file{ success_path };
	success_file.close();

	return true;

}

void Workflow::mapProcess(MapBase<std::string, std::string>* mapClass, int m) {

	string holder2;
	string fileName;
	FileMan* fileTester;
	BOOST_LOG_TRIVIAL(info) << "Map thread #  " << m;
	fileTester = &fileObject[m];
	for (auto& entry : file_partitions[m]) {
		//printing for testing
	//also do generic_string() as it is a filepath not a string
		BOOST_LOG_TRIVIAL(info) << entry.generic_string() << endl;

		//grabs the entire thing and puts each line into a vector
		fileTester->grabFile(entry.generic_string());


		for (int i = 0; i < fileTester->vecSize; i++) {
			holder2 = fileTester->readVecLine(i);

			//testing remnant
			//cout << i << endl;

			//Needs to be before the map
			//letting our export know to send in map
			if (i == fileTester->vecSize - 1) {
				mapClass->setLastLine(true);
			}
			//I shouldnt need to check for eof since the vector will only be so big
			mapClass->mapFunc(fileTester->getTemp(), holder2);
		}
		//reset values
		mapClass->clearBufferVector();


		//after completing a file clear the vector for the next file
		fileTester->resetVecSize();
		fileTester->resetVector();
	}
	//cout << "mapProcess Thread" << endl;
}

void Workflow::reduceProcess(ReduceBase<std::string>* reduceClass, int m) {
	Sorting sortTester; // Create sorter object
	FileMan* fileTester;
	BOOST_LOG_TRIVIAL(info) << "Reduce thread #  " << m;
	// Modify to partition
	fileTester = &fileObject[m];
	//now we sort the results and return them to reduce	
	vector<string> reduceData = sortTester.sortFile(fileTester->getTemp());

	BOOST_LOG_TRIVIAL(info) << "Running Reduce function for temp: " << fileTester->getTemp();

	for (int i = 0; i < reduceData.size(); i++) {
		//reduceTester->reduceFunc(reduceData[i]);
		// Create the reduce threads ( Currently only one so runs sequentially)		
		reduceClass->reduceFunc(reduceData[i], fileTester->getOut());
	}
	//after completing a file clear the vector for the next file
	fileTester->resetVecSize();
	fileTester->resetVector();
	BOOST_LOG_TRIVIAL(info) << "Reduce function completed for output: " << fileTester->getOut();
	//cout << "mapProcess Thread" << endl;
}

void Workflow::reduceAggregator(ReduceBase<std::string>* reduceClass, std::vector<string>& files, string outputDirectory) {
	//Sorting sortTester; // Create sorter object
	FileMan finalOutput, intermediateOutput;
	BOOST_LOG_TRIVIAL(info) << "\n" << endl;

	finalOutput.setOut(outputDirectory + "\\OUTPUT_FINAL.txt");
	finalOutput.clearFile(outputDirectory + "\\OUTPUT_FINAL.txt");

	for (int file = 0; file < files.size(); file++)
	{
		vector<string> bufferData;
		bufferData.clear();
		string temp;

		BOOST_LOG_TRIVIAL(info) << "reduceAggregator() agregating files " << files[file];
		intermediateOutput.grabFile(files[file]);
		for (int i = 0; i < intermediateOutput.vecSize; i++) {
			temp = intermediateOutput.readVecLine(i);
			bufferData.push_back(temp);
		}
		finalOutput.appendVectorToFile(finalOutput.getOut(), bufferData);
		//after completing a file clear the vector for the next file
		finalOutput.resetVecSize();
		finalOutput.resetVector();

		intermediateOutput.resetVecSize();
		intermediateOutput.resetVector();
	}
	BOOST_LOG_TRIVIAL(info) << "Combined output Written to : " << outputDirectory + "\\OUTPUT_FINAL.txt";

	// Jake to add aggregator function here
	//made a new standalone func can be put here and not ina  func if wanted
	
	reduceOutputAggregator(outputDirectory);

	//cout << "mapProcess Thread" << endl;
}

//reduces the output down
void Workflow::reduceOutputAggregator(string outputDirectory) {
	
	FileMan finalOutput, intermediateOutput;
	string temp2,wordHolder;
	vector<string> bufferData2;
	bufferData2.clear();
	std::unordered_map<string, int> tempMap;
	int valueHolder;
	size_t splitPos;
	

	finalOutput.setOut(outputDirectory + "\\OUTPUT_FINAL.txt");
	finalOutput.grabFile(outputDirectory + "\\OUTPUT_FINAL.txt");


	for (int i = 0; i < finalOutput.vecSize; i++) {
		//grab the line
		temp2 = finalOutput.readVecLine(i);

		//line needs to be split
		
		//this is dumb but there is an error with there being a blank line created when the files are merged
		//therefore we need to run this check. It could be optimized but this is just for making sure it works
		if (!temp2.empty()) {

			//splits the string into key "example" and value "10"
			splitPos = temp2.find(" ");
			wordHolder = temp2.substr(0, splitPos);
			valueHolder = stoi(temp2.substr(splitPos + 1, temp2.length()));

			//if it is already in the map add the value
			if (tempMap.contains(wordHolder)) {
				tempMap[wordHolder] = valueHolder + tempMap[wordHolder];
			}
			//else create a new key and value in the map
			else {
				tempMap.insert(make_pair(wordHolder, valueHolder));
			}
		}
	}

	//now we can clear the file as we have ran through it
	finalOutput.clearFile(outputDirectory + "\\OUTPUT_FINAL.txt");

	//for each value in the map we add it back to the buffer
	//we have to do this after or we get duplicates(however this can be optimized)
	for (const auto& [k, v] : tempMap) {
		//std::cout << k << " " << v << std::endl;
		bufferData2.push_back(k + " " + to_string(v));
	}
	//send the buffer to the outputfile
	finalOutput.appendVectorToFile(finalOutput.getOut(), bufferData2);

	BOOST_LOG_TRIVIAL(info) << "Reduced output Written to : " << outputDirectory + "\\OUTPUT_FINAL.txt";
}



