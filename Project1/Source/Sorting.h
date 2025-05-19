#ifndef SORTING_H
#define SORTING_H

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

using namespace std;


// Sort and aggregate the intermediate files
// Intermediate output file contain data as 
// Ex - (“a”, 1), (“the”, 1).
// 
// sortFile() function output will generate the intermediate data in the format,
// Ex - (“a”, [1, 1, …]), (“the”, [1, 1, …])

class Sorting
{
public:
	
	vector<string> sortFile(string fileName);

private:
	
};

#endif