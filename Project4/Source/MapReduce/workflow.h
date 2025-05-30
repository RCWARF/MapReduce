//	Removed all DLL references, added Comm controller
#pragma once
#include <boost/filesystem.hpp>
#include "mapBase.h"
#include "reduceBase.h"
#include "Comm.h"
#include "Sockets.h"
#include "Message.h"

class Workflow
{
public:
	Workflow(std::string input_dir_arg,
		std::string inter_dir_arg,
		std::string output_dir_arg,
		int num_mappers,
		int num_reducers,
		int controller_port,
		std::vector<std::string> stub_endpoints);								// constructor
	~Workflow();																// destructor
	boost::filesystem::path getTargetDir();										// directory containing files to be fed to Map
	boost::filesystem::path getIntermediateDir();								// directory containing intermediate files from Map
	boost::filesystem::path getOutDir();										// directory containing output file(s) from reduce
	void runMapReduceWf();														// Runs the map sort reduce workflow via stubs


private:
	boost::filesystem::path target_dir_;
	boost::filesystem::path intermediate_dir_;
	boost::filesystem::path out_dir_;

	// Configurable number of map and reduce threads
	int num_mappers_;
	int num_reducers_;

	// Controller socket
	Sockets::SocketSystem ss_;
	MsgPassingCommunication::EndPoint* endpoint_;
	MsgPassingCommunication::Comm* controller_;
	std::vector<MsgPassingCommunication::EndPoint> stubs_;

	// Validators and setters
	void setInputDirectory(std::string input_dir_arg);
	void setTempDirectory(std::string temp_dir_arg);
	void setOutputDirectory(std::string output_dir_arg);
	void startController(int port, std::vector<std::string> stub_endpoints);

	// Partitions files into N partitions/groups
	static std::vector<std::vector<boost::filesystem::path>> partitionFiles(const std::vector<boost::filesystem::path>& files, int partitions);

	// Creates a message for running a map process
	MsgPassingCommunication::Message createMapMessage(const MsgPassingCommunication::EndPoint& destination, 
		const std::vector<boost::filesystem::path>& files, 
		int num_partitions);


	// Creates a message for running a reduce process
	MsgPassingCommunication::Message createReduceMessage(const MsgPassingCommunication::EndPoint& destination, 
		const std::vector<boost::filesystem::path>& files, 
		const boost::filesystem::path& output_directory, 
		int partition);

	MsgPassingCommunication::Message createStopMessage(const MsgPassingCommunication::EndPoint& destination,
		const std::vector<boost::filesystem::path>& files,
		int partitions);
};

