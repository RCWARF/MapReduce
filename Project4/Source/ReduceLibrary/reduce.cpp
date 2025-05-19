#include "reduce.h"
#include "framework.h"

Reduce::Reduce(const boost::filesystem::path& directory) : ReduceBase<std::string, int>(directory)
{
}

// Performs reduce operation on intermediate results.
// Given sorted data from the intermediate file, reduces the results 
// by aggregating the values and output result to a file.
// e.g. (word, [1, 1]) -> (word, 2)
// If reduce operation fails (cannot write to file), returns -1. 
// If reduce operation succeeds, returns 0.
int Reduce::reduceFunc(const std::string& key, const std::vector<int>& values)
{
	// Run reduce
	int result = std::reduce(std::execution::seq, values.begin(), values.end());
	// Write reduced value
	return exportFunc(key, result);
}

REDUCELIBRARY_API Reduce* createReducer(const boost::filesystem::path& directory)
{
	return new Reduce(directory);
}

REDUCELIBRARY_API void destoryReducer(const Reduce* reducer)
{
	delete reducer;
}
