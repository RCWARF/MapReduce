// Map class and library definition.
#pragma once
#include "framework.h"
#include <string>
#include <vector>

#ifdef MAPLIBRARY_EXPORTS
#define MAPLIBRARY_API __declspec(dllexport)
#else
#define MAPLIBRARY_API __declspec(dllimport)
#endif

using namespace std;

class Map : public MapBase<std::string, std::string>
{

public:
	//create a buffer to export all the data in one go
	vector<string> bufferData;


	//there has to be a better way to detect end of mapping to then export the buffer but meh whatever
	bool lastLine = false;
	bool lastWord = false;

	//change the bool for the last line
	void setLastLine(bool boolVal) {
		lastLine = boolVal;
	}

	//clears the vector
	void clearBufferVector() {
		bufferData.clear();
	}

	//pass temp func and the line of words
	void mapFunc(string fileName, string temp);


	//I have no idea how else to export this data. I mean i guess you can return it to main? after that then export it
	//Doing that doesn't make sense since you would just return in the map function. This would then cause the export function to be redundent
	//it works idk
	void exportFunc(string fileName, string temp);

};

// Map Library factory - exposes functions to create and destory Map objects
extern "C" MAPLIBRARY_API Map * _cdecl createMapper();
extern "C" MAPLIBRARY_API void _cdecl destoryMapper(const Map * mapper);