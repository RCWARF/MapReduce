#ifndef FILE_MANAGEMENT
#define FILE_MANAGEMENT

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>


using namespace std;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::getline;
using std::vector;
using std::stringstream;
namespace fs = std::experimental::filesystem;

//FILE MANAGEMENT CLASS
class FileMan {
public:
	string inputFilePath, outputFilePath, tempFilePath;
	size_t vecSize;

	//vector to hold the entire file. should shorten runtime
	vector<string> rawFile;

	//constructor
	FileMan() {
		vecSize = 0;
	}
	//mutators
	string getInput() {
		return inputFilePath;
	}

	void setInput(string temp) {
		inputFilePath = temp;
	}

	string getOut() {
		return outputFilePath;
	}

	void setOut(string temp) {
		outputFilePath = temp;
	}

	string getTemp() {
		return tempFilePath;
	}

	void setTemp(string temp) {
		tempFilePath = temp;
	}


	//both are used with readLine to determine the line number we want.
	void resetVector() {
		rawFile.clear();
	}

	void resetVecSize() {
		vecSize = 0;
	}

	//Read a single line from the vector
	//we pass the number we want to read
	string readVecLine(int lineNum);


	//writes the entirity of a vector to a file
	void appendVectorToFile(string filePath, vector<string>lineData);

	//writes the entirity of a vector to a file
	void overWriteVectorToFile(string filePath, vector<string>lineData);


	
	//grab file and parse it into the vector
	void grabFile(string filePath);

	//Append to a file
	void appendFile(string filePath, string temp);

	//remove everything and only write the string you want to it
	void overwriteFile(string filePath, string temp);
	

	//remove everything from a file
	void clearFile(string filePath);

};

#endif
