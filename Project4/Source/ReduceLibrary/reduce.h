#pragma once
#include "framework.h"

#ifdef REDUCELIBRARY_EXPORTS
#define REDUCELIBRARY_API __declspec(dllexport)
#else
#define REDUCELIBRARY_API __declspec(dllimport)
#endif


class Reduce : public ReduceBase<std::string, int>
{

public:
	Reduce(const boost::filesystem::path& directory);

	// Performs reduce operation on intermediate results.
	// Given sorted data from the intermediate file, reduces the results 
	// by aggregating the values and output result to a file.
	// e.g. (word, [1, 1]) -> (word, 2)
	// If reduce operation fails (cannot write to file), returns -1. 
	// If reduce operation succeeds, returns 0.
	int reduceFunc(const std::string& key, const std::vector<int>& values);
};

extern "C" REDUCELIBRARY_API Reduce * createReducer(const boost::filesystem::path & directory);
extern "C" REDUCELIBRARY_API void destoryReducer(const Reduce * reducer);