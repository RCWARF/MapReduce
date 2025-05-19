#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include "Reduce.h"

using std::stringstream;
using std::vector;
using std::string;
using std::to_string;
using std::getline;
using std::cout;


void Reduce::reduceFunc(string keyString) {

	//total number of 1's
	size_t total = 0;
	
	//this is to move through the string in the while loop
	size_t finder = 0;

	//tokenize the string.
	//Find length of given variable
	size_t n = keyString.length();


	//Check until we hit ' '
	//which means it runs until it hits a space		
	//when we his a space we subtract the current position from the total size of the string
	
	while (keyString[finder] != ' ') {
		//increase at start of function as when we hit ' ' we break out
		finder++;
		//now we are at position so we move add one to find out how many 1's their are
		total = n - (finder + 1);
	}

	exportFunc(keyString.substr(0,finder),total);


}

void Reduce::exportFunc(string key, size_t numResult) {
	FileMan reductManager;
	
	string temp = key + " " + to_string(numResult);

	//write values to file
	reductManager.appendFile((outDir + "\\output.txt"), temp);

	//write sucess
	reductManager.overwriteFile((outDir + "\\SUCCESS.txt"),"SUCCESS");

}
