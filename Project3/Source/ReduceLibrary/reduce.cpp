#include "reduce.h"
#include "framework.h"
#include "FileMgr.h"

Reduce::Reduce(std::string dir) {
	outDir = dir;
}

void Reduce::reduceFunc(string keyString, string fileName) {

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

	exportFunc(keyString.substr(0, finder), total, fileName);


}

void Reduce::exportFunc(string key, size_t numResult, string fileName) {
	FileMan reductManager;

	string temp = key + " " + to_string(numResult);

	//write values to file		
	reductManager.appendFile(fileName, temp);

}


REDUCELIBRARY_API Reduce* createReducer(std::string dir)
{
	return new Reduce(dir);
}

REDUCELIBRARY_API void destoryReducer(const Reduce* reducer)
{
	delete reducer;
}
