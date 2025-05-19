#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include <string>


using namespace std;

class Executive
{
private:
	string inputDirectory;
	string outputDirectory;
	string tempDirectory;
public:
	
	bool validateUserInputs();
	int startWorkflow(void);	// kick off Workflow run																			

private:
	
};

#endif