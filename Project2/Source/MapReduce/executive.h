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

	bool validateUserInputs();
	int startWorkflow(void);	// kick off Workflow run																			

private:
};
