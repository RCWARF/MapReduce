#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <string>
#include <iostream>
#include <sys/stat.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <filesystem>
#include <string>

using namespace std;


class Workflow {
	
public:
	bool runMapReduceWf(std::string, std::string, std::string);
};

#endif