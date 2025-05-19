#pragma once
#include "framework.h"

/*	Use either dllexport or dllimport depending
	on whether REDUCELIBRARY_EXPORTS is defined
*/
#ifdef REDUCELIBRARY_EXPORTS
#define REDUCELIBRARY_API __declspec(dllexport)
#else
#define REDUCELIBRARY_API __declspec(dllimport)
#endif

using namespace std;

class Reduce : public ReduceBase<std::string>
{

public:
	//this is needed as how else would the export function know where to export. 
	string outDir;

	Reduce(string dir);

	//Hashing will be the fastest way to completed this
	void reduceFunc(string keyString, string fileName);

	void exportFunc(string key, size_t numResult, string fileName);
};

extern "C" REDUCELIBRARY_API Reduce * _cdecl createReducer(std::string);
extern "C" REDUCELIBRARY_API void _cdecl destoryReducer(const Reduce * reducer);