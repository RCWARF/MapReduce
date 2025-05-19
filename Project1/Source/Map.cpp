#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>
#include "Map.h"


//pass temp func and the line of words
void Map::mapFunc(string fileName, string temp) {
	// Find length of given variable
	size_t n = temp.length();

	// Create an empty string
	string word = "";
	//PARSING
	// Iterate over the string character by character using
	// For loop
	//first for loop handles finding unsigned characters and passing them along,
	//as well as removes punctuation
	for (int i = 0; i < n; i++) {
		if (static_cast<unsigned char>(temp[i]) > 127) {
			temp[i] = temp[i];
		}

		else if(ispunct(temp[i])) {
			
			temp.erase(i--, 1);
			n = temp.length();
		}
	}
	
	for (int i = 0; i < n; i++) {

		// Check if the current iteration is equal to ' '
		// which means it runs until it hits a space		
		if (temp[i] == ' ') {

			word += tolower(temp[i]);



			word = word + " 1";

			//just incase we change this bool
			lastWord = false;
			//export
			exportFunc(fileName, word);
			
			//clear the word
			word = "";
		}
		//checks if its the last char in the line
		else if(i == (n - 1)) {

			word += tolower(temp[i]);

			word = word + " 1";

			//let it know w are on the last word
			lastWord = true;
			//export
			exportFunc(fileName, word);
			//clear the word
			word = "";
		}
		// Add current character in word string
		else {
			word += tolower(temp[i]);
		}
	}

}

//I have no idea how else to export this data. I mean i guess you can return it to main? after that then export it
	//Doing that doesn't make sense since you would just return in the map function. This would then cause the export function to be redundent
	//it works idk
void Map::exportFunc(string fileName, string temp) {
	FileMan dontLike;

	
	if (lastWord && lastLine) {
		bufferData.push_back(temp);
		dontLike.appendVectorToFile(fileName,bufferData);
		lastWord = false;
		lastLine = false;
	}
	else {
		bufferData.push_back(temp);
	}
	


	//dontLike.appendFile(fileName, temp);

}
