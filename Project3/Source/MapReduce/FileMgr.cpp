#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "FileMgr.h"

//Read a single line from the vector
string FileMan::readVecLine(int lineNum) {
	return(rawFile[lineNum]);
}


void FileMan::grabFile(string filePath) {
	string idk = "\0";
	string temp;
	int currentLineNum = 0;
	ifstream myfile;
	myfile.open(filePath);

	while (!myfile.eof()) {
		getline(myfile, temp);
		//add file may be better to use the buffer instead of sending to string temp
		//i believe fgets would do so 
		rawFile.push_back(temp);
	}
	//file has been read
	currentLineNum = 0;
	myfile.close();

	vecSize = rawFile.size();
}

//appends a vector to the file
void FileMan::appendVectorToFile(string filePath, vector<string>lineData) {
	ofstream myfile;
	myfile.open(filePath, std::ios_base::app);
	for (int i = 0; i < lineData.size(); i++) {
		myfile << lineData[i] << endl;
	}
	myfile.close();
}

//overwrites the vector to the file
void FileMan::overWriteVectorToFile(string filePath, vector<string>lineData) {
	ofstream myfile;
	myfile.open(filePath);
	for (int i = 0; i < lineData.size(); i++) {
		myfile << lineData[i] << endl;
	}
	myfile.close();
}



//Append to a file
void FileMan::appendFile(string filePath, string temp) {
	ofstream myfile;
	myfile.open(filePath, std::ios_base::app);
	myfile << temp << endl;
	myfile.close();
}

//remove everything and only write the string you want to it
void FileMan::overwriteFile(string filePath, string temp) {
	ofstream myfile;
	myfile.open(filePath);
	myfile << temp << endl;
	myfile.close();
}

//remove everything from a file
void FileMan::clearFile(string filePath) {
	ofstream myfile;
	myfile.open(filePath);
	myfile << "";
	myfile.close();
}
