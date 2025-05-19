#include <string>
#include "workflow.h"
#pragma once

using namespace std;

class Executive
{
private:
	string inputDirectory;
	string outputDirectory;
	string tempDirectory;
	string mapDLL;
	string reduceDLL;

public:

	bool validateInputDirectory();
	bool validateTempDirectory();
	bool validateOutputDirectory();
	bool validateMapDllPath();
	bool validateReduceDllPath();

	int startWorkflow(void);	// kick off Workflow run																			

private:
};
