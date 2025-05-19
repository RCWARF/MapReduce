#include <array>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#pragma once

template <typename KEYT, typename VALUET>
class MapBase {

public:
	MapBase(const boost::filesystem::path& directory);

	~MapBase();

	// Key = file name, value = Line of text from file. Returns 0 on success. 
	// Returns -1 on failure.
	//
	virtual int mapFunc(const KEYT& key, const VALUET& value) = 0;

	// Gets the path of the most recently written to output file  
	boost::filesystem::path getOutputPath();

	// Gets the output directory path 
	boost::filesystem::path getOutputDirectory();

private:
	boost::filesystem::path output_directory_;
	boost::filesystem::path output_path_;
	boost::filesystem::ofstream* output_stream_;
	std::array<char, 2048> output_buffer_;

protected:
	// Exports word, value pair to file
	// If the export fails, returns -1. 
	// If the export succeeds, returns 0.
	// Default implmentation. Can be overriden.
	virtual int exportFunc(const std::string& filename,
		const KEYT& key,
		int value);
};

template<typename KEYT, typename VALUET>
inline MapBase<KEYT, VALUET>::MapBase(const boost::filesystem::path& directory)
{
	output_directory_ = directory;
	output_stream_ = new boost::filesystem::ofstream{};
	output_stream_->rdbuf()->pubsetbuf(&output_buffer_.front(),
		output_buffer_.size());
}

template<typename KEYT, typename VALUET>
inline MapBase<KEYT, VALUET>::~MapBase()
{
	// Flush & close the file stream
	if (output_stream_->is_open()) {
		output_stream_->close();
	}
	delete output_stream_;
}

// Getter for OutputPath data member
template<typename KEYT, typename VALUET>
inline boost::filesystem::path MapBase<KEYT, VALUET>::getOutputPath()
{
	return output_path_;
}

// Getter for OutputDirectory data member
template<typename KEYT, typename VALUET>
inline boost::filesystem::path MapBase<KEYT, VALUET>::getOutputDirectory()
{
	return output_directory_;
}

//	Export to key, value pair to file
template<typename KEYT, typename VALUET>
inline int MapBase<KEYT, VALUET>::exportFunc(const std::string& filename, const KEYT& key, int value)
{
	boost::filesystem::path filePath = boost::filesystem::path{
		output_directory_.string() + "\\" + filename
	};

	// Started to process a new file
	if (output_path_.compare(filePath) != 0) {
		// Flush & close the old file stream and start a new one
		if (output_stream_->is_open()) {
			output_stream_->close();
		}
		output_path_ = filePath;
		output_stream_->open(output_path_);
	}

	// There is a problem with the output file
	if (output_stream_->fail()) {
		return -1;
	}
	else {
		// Write to the file stream 
		*(output_stream_) << "(";
		*(output_stream_) << key;
		*(output_stream_) << ", ";
		*(output_stream_) << value;
		*(output_stream_) << ")\n";
		return 0;
	}
}