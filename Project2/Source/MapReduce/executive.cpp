#include <string>
#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <boost\log\core.hpp>
#include <boost\log\expressions.hpp>
#include <boost\filesystem.hpp>
#include <boost\log\trivial.hpp>
#include "executive.h"
#include "workflow.h"

using namespace std;

bool Executive::validateUserInputs() {
	// check if input is a valid directory
	struct stat buffer;

	if (stat(inputDirectory.c_str(), &buffer) != 0) {
		BOOST_LOG_TRIVIAL(error) << "Input " << inputDirectory << " directory does not exist!";
		//cout << "Input " << inputDirectory << " directory does not exist!" << endl;
		return false;
	}

	if (filesystem::is_empty(inputDirectory)) {
		BOOST_LOG_TRIVIAL(error) << "Input directory is empty";
		//cout << "Input directory is empty" << endl;
		return false;
	}

	if (stat(tempDirectory.c_str(), &buffer) != 0) {
		BOOST_LOG_TRIVIAL(error) << "Temp " << tempDirectory << " directory does not exist!";
		//cout << "Temp " << tempDirectory << " directory does not exist!" << endl;
		return false;
	}

	if (stat(outputDirectory.c_str(), &buffer) != 0) {
		BOOST_LOG_TRIVIAL(error) << "Output " << outputDirectory << " directory does not exist!";
		//cout << "Output " << outputDirectory << " directory does not exist!" << endl;
		return false;
	}

    if (stat(mapDLL.c_str(), &buffer) != 0) {
        BOOST_LOG_TRIVIAL(error) << "Map DLL " << mapDLL << " file does not exist!";
        //cout << "Output " << outputDirectory << " directory does not exist!" << endl;
        return false;
    }


    if (stat(reduceDLL.c_str(), &buffer) != 0) {
        BOOST_LOG_TRIVIAL(error) << "Reduce DLL " << reduceDLL << " file does not exist!";
        //cout << "Output " << outputDirectory << " directory does not exist!" << endl;
        return false;
    }

	return true;
}



int Executive::startWorkflow(void)
{
	Workflow wf;
	bool wfResult;
	bool fileStatus;
    // declare strings to hold command line input values which should each be a path to a directory

    // Prompt the user for inputs 
    cout << "\nEnter the following inputs for the Map Reduce Project1: " << endl;


    cout << "Enter the input directory path: ";
    getline(cin, inputDirectory);

    cout << "Enter the temp directory path: ";
    getline(cin, tempDirectory);

    cout << "Enter the output directory path: ";
    getline(cin, outputDirectory);

    cout << "Enter the map dll path: ";
    getline(cin, mapDLL);

    cout << "Enter the reduce dll path: ";
    getline(cin, reduceDLL);

	fileStatus = validateUserInputs();
	if (!fileStatus) {
		cout << "Command line inputs are invalid!" << endl;
		return -1;
	}

    cout << "Entered paths are valid:\n" << endl;
    //cout << inputDirectory << endl << tempDirectory << endl << outputDirectory << endl << mapDLL << reduceDLL << endl;
    cout << "\n";

	// if checks pass, dirs are valid, pass them off to the workflow
	wfResult = wf.runMapReduceWf(inputDirectory, tempDirectory, outputDirectory, mapDLL, reduceDLL);
	if (!wfResult) {
		cout << "Workflow failed to run!" << endl;
		return -1;
	}
	cout << "Successfully Terminated" << endl;
	return 0;
}

// Set the boost logging threshold to >= info
void init()
{
	cout << "\t" << "#############################################################" << endl;
	cout << "\t" << "############### Welcome To Map Reduce Project2 ###############" << endl;
	cout << "\t" << "#############################################################" << endl;

	boost::log::core::get()->set_filter
	(
		boost::log::trivial::severity >= boost::log::trivial::info
	);
}

int main(int argc, char* argv[]) {
	init();
	Executive exec;
	exec.startWorkflow();
}