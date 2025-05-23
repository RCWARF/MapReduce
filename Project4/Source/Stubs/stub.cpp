#include <boost\log\trivial.hpp>
#include <thread>
#include <chrono>
#include <future>
#include "stub.h"
#include "sorting.h"
#include "Comm.h"
#include "Sockets.h"
#include "Message.h"

Stub::Stub(std::string endpoint,
	const CREATMAP& create_mapper,
	const CREATREDUCE& create_reducer)
{

	endpoint_ = parseEndpoint(endpoint);

	// Start the stub
	try {
		comm_ = new MsgPassingCommunication::Comm(endpoint_, endpoint_.toString());
		comm_->start();
		BOOST_LOG_TRIVIAL(info) << "Started stub at " << endpoint_.toString();
	}
	catch (const std::exception&) {
		// Failed to start stub
		BOOST_LOG_TRIVIAL(fatal) << "Unable to start stub at " << endpoint_.toString();
		exit(1);
	}


	// Save the DLLs functions
	create_map_ = create_mapper;
	create_reduce_ = create_reducer;
}

Stub::~Stub()
{
	// Shutdown the socket
	comm_->stop();
}


// Gets the stub's endpoint
const MsgPassingCommunication::EndPoint& Stub::getEndpoint() const
{
	return endpoint_;
}

// Listen for incoming map and reduce requests
void Stub::runStub()
{
	BOOST_LOG_TRIVIAL(info) << endpoint_.toString() << " started running";
	MsgPassingCommunication::Message msg;
	while (true) {
		BOOST_LOG_TRIVIAL(info) << endpoint_.toString() << " waiting for message.";
		msg = comm_->getMessage();
		BOOST_LOG_TRIVIAL(info) << endpoint_.toString() << "received messsage.";

		BOOST_LOG_TRIVIAL(debug) << msg.toString();
	
		std::unordered_map<std::string, std::string> attrs = msg.attributes();
		// Map request
		if (msg.command().compare("run_map") == 0 
			&& msg.containsKey("output_directory") 
			&& msg.containsKey("input_files")
			&& msg.containsKey("partitions")) {
			BOOST_LOG_TRIVIAL(info) << endpoint_.toString() << "received map request.";
			BOOST_LOG_TRIVIAL(info) << "map - input files:" << attrs["input_files"];
			BOOST_LOG_TRIVIAL(info) << "map - output directory:" << attrs["output_directory"];
			BOOST_LOG_TRIVIAL(info) << "map - number of partitions:" << attrs["partitions"];
			// Parse the message
			boost::filesystem::path output_dir {attrs["output_directory"]};
			int partitions = std::stoi(attrs["partitions"]);
			std::vector<boost::filesystem::path> input_files = parseFileList(attrs["input_files"]);

			// Start the map thread
			std::thread map_thread = std::thread(&Stub::MapProcess, this, input_files, output_dir, partitions, parseEndpoint(attrs["from"]));
			map_thread.detach();
			BOOST_LOG_TRIVIAL(info) << "Started map operation.";
		}		

		// Reduce request
		if (msg.command().compare("run_reduce") == 0
			&& msg.containsKey("output_directory")
			&& msg.containsKey("input_files")
			&& msg.containsKey("partition")) {
			BOOST_LOG_TRIVIAL(info) << endpoint_.toString() << "received reduce request.";
			BOOST_LOG_TRIVIAL(info) << "reduce - input files:" << attrs["input_files"];
			BOOST_LOG_TRIVIAL(info) << "reduce - output directory:" << attrs["output_directory"];
			BOOST_LOG_TRIVIAL(info) << "reduce - partition:" << attrs["partition"];

			// Parse the message
			boost::filesystem::path output_dir{ attrs["output_directory"] };
			int partition = std::stoi(attrs["partition"]);
			std::vector<boost::filesystem::path> input_files = parseFileList(attrs["input_files"]);

			// Start the reduce thread
			std::thread reduce_thread = std::thread(&Stub::ReduceProcess, this, input_files, output_dir, partition, parseEndpoint(attrs["from"]));
			reduce_thread.detach();
			BOOST_LOG_TRIVIAL(info) << "Started reduce operation.";
		}

		if (msg.command().compare("stop_stub") == 0) {
			BOOST_LOG_TRIVIAL(info) << "Client terminiated.";
			break;
		}
	}
}

void Stub::stopStub()
{
	// Shutdowns the socket
	comm_->stop();
}


// A hearbeat thread
// Sends a heartbeat message to the given endpoint at the given interval
// Reference: https://www.tutorialspoint.com/how-do-i-terminate-a-thread-in-cplusplus11
void Stub::heartbeatThread(MsgPassingCommunication::EndPoint client_endpoint, 
	int interval,
	std::string message,
	std::future<void> future)
{
	while (future.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
		comm_->postMessage(createHeartbeatMessage(client_endpoint, message));
		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	}
}

// A heartbeat message
MsgPassingCommunication::Message Stub::createHeartbeatMessage(MsgPassingCommunication::EndPoint client_endpoint,
	std::string message)
{
	MsgPassingCommunication::Message msg;
	msg.to(client_endpoint);
	msg.from(endpoint_);
	msg.name("heartbeat");
	msg.attribute("message", message);
	return msg;
}

// A success message
// Sent when map or reduce process completes
MsgPassingCommunication::Message Stub::createSuccessMessage(MsgPassingCommunication::EndPoint client_endpoint, 
	std::string message)
{
	MsgPassingCommunication::Message msg;
	msg.to(client_endpoint);
	msg.from(endpoint_);
	msg.name("success");
	msg.attribute("message", message);
	return msg;
}


// A failure message
// Sent when map or reduce fails
MsgPassingCommunication::Message Stub::createFailureMessage(MsgPassingCommunication::EndPoint client_endpoint, 
	std::string message)
{
	MsgPassingCommunication::Message msg;
	msg.to(client_endpoint);
	msg.from(endpoint_);
	msg.name("failure");
	msg.attribute("message", message);
	return msg;
}

// Runs a map operation
void Stub::MapProcess(const std::vector<boost::filesystem::path>& files, 
	const boost::filesystem::path& output_directory, 
	int num_partitions,
	MsgPassingCommunication::EndPoint client_endpoint)
{
	// Get the id of the thread
	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::string thread_id = ss.str();

	// Start the heartbeat thread
	std::promise<void> signal_exit;
	std::future<void> future = signal_exit.get_future();
	std::thread heartbeat_thread(&Stub::heartbeatThread, this, client_endpoint, 2000, "map thread: " + thread_id, std::move(future));


	// Instantiate a Map object via the IMap interface
	MapBase<std::string, std::string>* mapper = create_map_(output_directory);

	// Partition the input files
	std::vector<std::vector<boost::filesystem::path>> partitions = partitionFiles(files, num_partitions);


	// Process the files in each partition with map
	boost::filesystem::path input_file_path;
	boost::filesystem::ifstream input_stream;
	std::string line;
	for (int partition = 0; partition < partitions.size(); partition++) {
		// Output file name is thread id + p{parition number}
		std::string partition_name = thread_id + "p" + std::to_string(partition);

		for (int file = 0; file < partitions[partition].size(); file++) {
			// Open the input file
			boost::filesystem::path current_file = partitions[partition][file];
			input_stream.open(current_file);

			// File could not be opened
			if (input_stream.fail() == 1)
			{

				std::string error = "Error in map: ifstream could not be opened for " + current_file.filename().string();
				// Send error message back to controller
				BOOST_LOG_TRIVIAL(fatal) << error;
				comm_->postMessage(createFailureMessage(client_endpoint, error));
				// Signal the heartbeat thread to stop
				signal_exit.set_value();
				heartbeat_thread.join();
			}

			int success = 0;
			int line_count = 0;
			BOOST_LOG_TRIVIAL(info) << "Running map process for " << current_file.filename().string();
			// Process all lines of the file via map
			while (std::getline(input_stream, line))
			{

				for (int i = 0; i < line.size(); i++) {

					// Finding the character whose
					// ASCII value fall under this
					// range
					if ((static_cast<unsigned char>(line[i])) > 159) {
						// erase function to erase
						// the character
						//BOOST_LOG_TRIVIAL(info) << "Deleted character 194 and 160";
						line[i] = ' ';
						//line.erase(i, 1);
						//i--;
					}
				}
		
				// Increment the line count and call the map object to map the key and the value
				line_count++;
				success = mapper->mapFunc(partition_name, line);

				// If the map member function of the map object does not return zero (which is a success), then log
				if (success != 0)
				{
					// Send error message back to controller
					std::string error = "Error in map: Map did not successfully process entire file at line " + std::to_string(line_count) + " of " + current_file.filename().string();
					BOOST_LOG_TRIVIAL(fatal) << error;
					comm_->postMessage(createFailureMessage(client_endpoint, error));

					// Signal the heartbeat thread to stop
					signal_exit.set_value();
					heartbeat_thread.join();
				}
			}

			// Log map process has completed for the current file
			BOOST_LOG_TRIVIAL(info) << "Map process completed for " << current_file.filename().string();

			// Close the current input stream
			input_stream.close();
			line_count = 0;
		}
	}

	// Signal the heartbeat thread to stop
	signal_exit.set_value();
	heartbeat_thread.join();

	// Sent success message to client
	std::string complete_message = "Map process " + thread_id + " complete.";
	BOOST_LOG_TRIVIAL(info) << complete_message;
	comm_->postMessage(createSuccessMessage(client_endpoint, complete_message));
	delete mapper;
}

// Takes as input all the files that belong to a particular partition along with the partition id (e.g 0) and the directory to output the reduce file to.
// Because we are running multiple reducers, we will get multiple reduce files at the end when the threads return.
// We will have to run one final reduce process over all those intermediate files e.g. reduce0.txt, reduce1.txt if we had 2 partitions
// The intermeidate reduce files should go in the intermediate dir, not the final output directory, which is why there is an output directory parameter.
void Stub::ReduceProcess(const std::vector<boost::filesystem::path>& files, 
	const boost::filesystem::path& output_directory, 
	int partition,
	MsgPassingCommunication::EndPoint client_endpoint)
{

	// Get the id of the thread
	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::string thread_id = ss.str();

	// Start the heartbeat thread
	std::promise<void> signal_exit;
	std::future<void> future = signal_exit.get_future();
	std::thread heartbeat_thread(&Stub::heartbeatThread, this, client_endpoint, 2000, "reduce thread: " + thread_id, std::move(future));

	// Create reducer
	// The intermeidate reduce files should go in the intermediate dir, not the final output directory, which is why there is an output directory parameter.
	ReduceBase<std::string, int>* reducer = create_reduce_(output_directory);
	// Set the output file name to reduce + partition e.g. reduce0.txt
	std::string output_filename = "reduce" + std::to_string(partition) + ".txt";
	reducer->setOutputFileName(output_filename);

	// Run sort on all the files that belong to the partition
	int sort_success = 0;
	Sorting* sorter = new Sorting();

	for (int file = 0; file < files.size(); file++)
	{
		BOOST_LOG_TRIVIAL(info) << "Running sort on " << files[file].filename().string();
		sort_success = sorter->sortFile(files[file]);

		if (sort_success != 0) {
			// TODO: Change to message to controller
			std::string error = "Failed to process " + files[file].filename().string() + " with sort.";
			BOOST_LOG_TRIVIAL(fatal) << error;
			comm_->postMessage(createFailureMessage(client_endpoint, error));
		}
	}

	// Run reduce on the output from sort
	BOOST_LOG_TRIVIAL(info) << "Running reduce operation.";
	int reducer_success = 0;
	for (auto const& pair : sorter->getAggregateData())
	{
		reducer_success = reducer->reduceFunc(pair.first, pair.second);
		if (reducer_success != 0) {
			std::string error = "Failed to export to " + reducer->getOutputPath().string() + " with reduce.";
			BOOST_LOG_TRIVIAL(fatal) << error;
			comm_->postMessage(createFailureMessage(client_endpoint, error));
		}
	}

	// Signal the heartbeat thread to stop
	signal_exit.set_value();
	heartbeat_thread.join();

	// Send message to client telling them the operation finished
	std::string complete_message = "Reduce process " + thread_id + " complete.";
	BOOST_LOG_TRIVIAL(info) << complete_message;
	comm_->postMessage(createSuccessMessage(client_endpoint, complete_message));

	// Delete reducer
	delete sorter;
	delete reducer;
}

// Partitions/groups a list of files/paths into the given number of partitions
// If there are less files than the number of partitions, partitions may be empty
std::vector<std::vector<boost::filesystem::path>> Stub::partitionFiles(const std::vector<boost::filesystem::path>& files,
	int partitions)
{
	std::vector<std::vector<boost::filesystem::path>> file_partitions(partitions);

	int partition = 0;
	// Place one file in a partition at a time
	// If there are less files than the number of partitions, some partitions will be empty
	for (int i = 0; i < files.size(); i++) {
		file_partitions[partition].push_back(files[i]);
		partition = (partition + 1) % partitions;
	}

	return file_partitions;
}

// Parses an endpoint string to an endpoint object e.g. localhost:8080
MsgPassingCommunication::EndPoint Stub::parseEndpoint(std::string endpoint)
{
	std::string stub_host;
	int stub_port;
	size_t pos = endpoint.find(':');
	if (pos != std::string::npos) {
		stub_host = endpoint.substr(0, pos);
		// Convert port to an integer
		try {
			stub_port = std::stoi(endpoint.substr(pos + 1));

		}
		catch (const std::exception&) {
			// Port is not an integer
			BOOST_LOG_TRIVIAL(fatal) << "Invalid endpoint " << endpoint;
			exit(1);
		}
		return MsgPassingCommunication::EndPoint(stub_host, stub_port);
	}
	else {
		BOOST_LOG_TRIVIAL(fatal) << "Invalid endpoint " << endpoint;
		exit(1);
	}
}

// Parses a list of comma seperated files
// CITE: https://www.tutorialspoint.com/parsing-a-comma-delimited-std-string-in-cplusplus
std::vector<boost::filesystem::path> Stub::parseFileList(std::string files)
{
	std::vector<boost::filesystem::path> file_paths;
	std::stringstream s_stream(files); 
	while (s_stream.good()) {
		std::string substr;
		std::getline(s_stream, substr, ','); 
		file_paths.push_back(boost::filesystem::path{ substr });
	}
	return file_paths;
}

