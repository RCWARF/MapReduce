// Template base class for generic Reduce DLLs
#include <string>
#include <vector>
#include <array>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#pragma once

template<typename KEYT>
class ReduceBase
{

public:
	virtual void reduceFunc(KEYT keyString, std::string fileName) = 0;	
};


