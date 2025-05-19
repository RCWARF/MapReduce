// Template base class for generic Map DLLs

#include <array>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#pragma once

template<typename KEYT, typename VALUET>
class MapBase {

public:
	//change the bool for the last line
	virtual void setLastLine(bool boolVal) = 0;

	//clears the vector
	virtual void clearBufferVector() = 0;

	//pass temp func and the line of words
	virtual void mapFunc(KEYT fileName, VALUET temp) = 0;
	
	virtual void exportFunc(KEYT fileName, VALUET temp) = 0;
};

