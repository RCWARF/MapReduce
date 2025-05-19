// ireduce.h
// Lyndsay LaBarge, Todd Hricik
// CSE687 Object Oriented Design
// May 12, 2022
// 
// Template class IReduce is a base clase that serves as an interface
// for the reduce library.
// 
// Abstract class.
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
	virtual void reduceFunc(KEYT keyString) = 0;	
};


