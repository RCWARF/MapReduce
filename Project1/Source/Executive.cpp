#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>

#include "Executive.h"
#include "Workflow.h"

using namespace std;

/**
 * @brief Print welcome banner
 *
 */
void printWelBanner()
{
	cout << "\t" << "#############################################################" << endl;
	cout << "\t" << "############### Welcome To Map Reduce Project1 ###############" << endl;
	cout << "\t" << "#############################################################" << endl;
}

bool Executive::validateUserInputs() {
	// check if input is a valid directory
	struct stat buffer;

	if (stat(inputDirectory.c_str(), &buffer) != 0) {
		cout << "Input " << inputDirectory << " directory does not exist!" << endl;
		return false;
	}

	if (filesystem::is_empty(inputDirectory)) {
		cout << "Input directory is empty" << endl;
		return false;
	}

	if (stat(tempDirectory.c_str(), &buffer) != 0) {
		cout << "Temp " << tempDirectory << " directory does not exist!" << endl;
		return false;
	}

	if (stat(outputDirectory.c_str(), &buffer) != 0) {
		cout << "Output " << outputDirectory << " directory does not exist!" << endl;
		return false;
	}
	
	return true;
}

int Executive::startWorkflow(void) {
	Workflow wf;
	bool wfResult;
	bool fileStatus;

	// Print the required program start banner.
	printWelBanner();
	// Prompt the user for inputs 
	cout << "\nEnter the following inputs for the Map Reduce Project1: " << endl;


	cout << "Enter the input directory path: ";
	getline(cin, inputDirectory);
	
	cout << "Enter the output directory path: ";
	getline(cin, outputDirectory);
	
	cout << "Enter the temp directory path: ";
	getline(cin, tempDirectory);
	
	fileStatus = validateUserInputs();
	if (!fileStatus) {
		cout << "Command line inputs are invalid!" << endl;
		return -1;
	}
	cout << "Entered paths are valid:\n" << endl;
	//cout << inputDirectory << endl << outputDirectory << endl << tempDirectory << endl;
	cout << "\n";

	// if checks pass, dirs are valid, pass them off to the workflow
	wfResult = wf.runMapReduceWf(inputDirectory, tempDirectory, outputDirectory);
	if (!wfResult) {
		cout << "Workflow failed to run!" << endl;
		return -1;
	}
	cout << "Successfully Terminated" << endl;
	return 0;
}

int main(int argc, char* argv[]) {
	Executive exec;
	exec.startWorkflow();
}
