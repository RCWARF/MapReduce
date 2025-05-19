// StubExecutive class that loads map and reduce DLLs 
// and starts stub threads at the given endpoints
#pragma once
#include <boost\filesystem.hpp>
#include <boost\log\trivial.hpp>
#include "mapBase.h"
#include "reduceBase.h"
#include "stub.h"
#include "Comm.h"
#include "Sockets.h"
#include "Message.h"

typedef MapBase<std::string, std::string>* (*CREATMAP)(const boost::filesystem::path&);
typedef ReduceBase<std::string, int>* (*CREATREDUCE)(const boost::filesystem::path&);

class StubExecutive
{
public:
	StubExecutive(std::string map_dll_file, 
		std::string reduce_dll_file,
		std::vector<std::string> stub_endpoints);
	~StubExecutive();
	int runStubs(std::vector<std::string> stub_endpoints);

private:
	Sockets::SocketSystem ss_;

	// Paths to map and reduce DLLs
	boost::filesystem::path map_lib_path_;
	boost::filesystem::path reduce_lib_path_;

	// Interfaces to map and reduce libraries
	CREATMAP create_map_;
	CREATREDUCE create_reduce_;

	// Handles to map and reduce DLLs
	HINSTANCE hDLL_map_;
	HINSTANCE hDLL_reduce_;

	// Acquire neccessary DLL handles for map and reduce libraries
	void getMapHandler(std::string pathToMapDLL);
	void getReduceHandler(std::string pathToReduceDLL);

	// Create the stubs
	void createStubs(std::vector<std::string> stub_endpoints);

	// Create a new stub and run it
	static void stubProc(std::string endpoint, const CREATMAP& map, const CREATREDUCE& reduce);
};