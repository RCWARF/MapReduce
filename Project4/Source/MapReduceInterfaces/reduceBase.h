#include <array>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#pragma once


template <typename KEYT, typename VALUET>
class ReduceBase
{

public:

	ReduceBase(const boost::filesystem::path& directory);

	~ReduceBase();

	virtual int reduceFunc(const KEYT& key, const std::vector<VALUET>& values) = 0;

	// Gets the path of the output file
	boost::filesystem::path getOutputPath();

	// Gets the path of the output directory
	boost::filesystem::path getOutputDirectory();

	void setOutputFileName(std::string name);

private:
	boost::filesystem::path output_directory_;
	boost::filesystem::path output_path_;
	boost::filesystem::ofstream* output_stream_;
	std::string output_file_name_ = "reduce.txt";

protected:
	virtual int exportFunc(const KEYT& key, VALUET value);
};

template<typename KEYT, typename VALUET>
inline ReduceBase<KEYT, VALUET>::ReduceBase(const boost::filesystem::path& directory)
{
	output_directory_ = directory;
	output_path_ = boost::filesystem::path{
		output_directory_.string() + "\\" + output_file_name_
	};
	output_stream_ = new boost::filesystem::ofstream{ };

	// Delete old result file if it exists
	if (boost::filesystem::exists(output_path_)) {
		boost::filesystem::remove(output_path_);
	}
}

// Destructor
template<typename KEYT, typename VALUET>
inline ReduceBase<KEYT, VALUET>::~ReduceBase()
{
	// Flush & close the file stream
	if (output_stream_->is_open()) {
		output_stream_->close();
	}
	delete output_stream_;
}

// Getter for output_path_ data member
template<typename KEYT, typename VALUET>
inline boost::filesystem::path ReduceBase<KEYT, VALUET>::getOutputPath()
{
	return output_path_;
}

// Getter for output_directory_ data member
template<typename KEYT, typename VALUET>
inline boost::filesystem::path ReduceBase<KEYT, VALUET>::getOutputDirectory()
{
	return output_directory_;
}

template<typename KEYT, typename VALUET>
inline void ReduceBase<KEYT, VALUET>::setOutputFileName(std::string name)
{
	// Update the output path
	output_file_name_ = name;
	output_path_ = boost::filesystem::path{
		output_directory_.string() + "\\" + output_file_name_
	};

	// Close old output stream if it is open
	if (output_stream_->is_open()) {
		output_stream_->close();
	}
	output_stream_ = new boost::filesystem::ofstream{ };


	// Delete old result file if it exists
	if (boost::filesystem::exists(output_path_)) {
		boost::filesystem::remove(output_path_);
	}
}

template<typename KEYT, typename VALUET>
inline int ReduceBase<KEYT, VALUET>::exportFunc(const KEYT& key, VALUET value)
{
	// Open the output_stream_ if it is not open
	if (!output_stream_->is_open()) {
		output_stream_->open(output_path_, std::ios_base::app);
	}

	// If output_stream_ does not fail to open, then output (key, reduced value) to output stream
	if (!output_stream_->fail()) {
		// Write result to disk 
		*(output_stream_) << "(";
		*(output_stream_) << key;
		*(output_stream_) << ", ";
		*(output_stream_) << value;
		*(output_stream_) << ")\n";
		output_stream_->close();
		return 0;
	}
	// Else return -1 to flag a failure
	else {
		return -1;
	}
}
