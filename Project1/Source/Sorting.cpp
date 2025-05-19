#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <unordered_map>
#include <filesystem>

#include "Sorting.h"
#include "FileMgr.h"

//sortfile returns a vector that is holding a pair of strings with a corresponding vector of 1's
vector<string> Sorting::sortFile(string fileName) {
	//iterator through file
	FileMan sortFileTester;
	//length of string
	size_t n;
	// Create an empty string
	string holder;
	string word;
	string outTemp;
	//mapping
	unordered_map<string, int> mp;

	//cout << "SORTING" << endl;
	//grabs the temp file and loads this into a vector
	sortFileTester.grabFile(fileName);

	for (int i = 0; i < sortFileTester.vecSize; i++) {
		holder = sortFileTester.readVecLine(i);

		//I shouldnt need to check for eof since the vector will only be so big
		//now for each line we manipulate it. to do "testWord" 1111



		//apply length
		n = holder.length();
		//in here so it gets reset
		word = "";
		// Iterate over the string character by character using
		// For loop
		for (int i = 0; i < n; i++) {

			// Check if the current iteration is equal to ' ' or
			// it's the last character
			if (holder[i] == ' ' || i == (n - 1)) {

				//add the word to the hash map and increment the corresponding value by 1
				mp[word]++;

				//this is just a test print, can be removed 
				//cout << word << endl;

				//do this to kick out of for loop
				//otherwise the loop will still run on the " 1" and will hit three times
				n = i;

			}

			// Add current character in word string
			else {
				word += holder[i];
			}
		}

	}


	//erase the temp file we had and now rewrite to it 
	sortFileTester.clearFile(fileName);


	//write each line as a vector to print out 
	//then just write to the file
	vector<string> outputVector;


	for (auto x : mp) {

		//this makes sort print 
		//test 11111

		outTemp = x.first + " ";
		for (int i = 0; i < x.second; i++) {
			outTemp += "1";
		}

		//add to the vector
		outputVector.push_back(outTemp);

	}
	//send the vector we made to temp
	sortFileTester.overWriteVectorToFile(fileName, outputVector);

	//cout << "DONE" << endl;

	return(outputVector);

}
