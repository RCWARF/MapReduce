#include <vector>
#include <boost/filesystem.hpp>
#include <boost/container/map.hpp>
#pragma once

// Sorts and aggregates data from an input file with the 
// following format...
//		(word, 1)
//		(word, 1)
//		...
// Data is aggregated, sorted, and stored within a map
// with the form...
//		(word, [1, 1])
class Sorting
{
public:
	// Sort map output from an input file.
	// Input file must have the following format
	//		(word, 1)
	//		(word, 1)
	//		...
	// If the file cannot be parsed, returns -1.
	// If the sort operation succeeds, retuns 0.
	int sortFile(const boost::filesystem::path& file);

	// Get aggregate/sorted data.
	// Aggregate data is map of where the key is a word
	// and the values are word counts. 
	// e.g. (word, [1, 1])
	const boost::container::map<std::string,
		std::vector<int>>&getAggregateData();

private:
	// Stores the sorted/aggregate data in memory
	boost::container::map<std::string, std::vector<int>> aggregate_data_;
};
