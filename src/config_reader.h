#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

/*
take in .txt file with the format option_name = value
return an unordered_map of option_name to value
*/

string remove_space(string s); //removes all space from a string
unordered_map<string, string> read(string file_name); 
//creates a map of stuff to the left side of the equals sign to stuff to the right side of the equals sign
string remove_end_space(string s); //removes spaces after the last non-space character of a string
string remove_beginning_space(string s); //removes spaces before the first non-space character of a string

//creates a map of stuff to the left side of the equals sign to stuff to the right side of the equals sign
unordered_map<string, string> read(string file_name){
	
	//intialize the map that will be returned
	unordered_map<string, string> map;
	
	//initialize a string for reading through the file
	string line;
	
	//initialize the file
	ifstream myfile(file_name);
	
	//makes sure the file was able to be opened
	if (myfile.is_open())
	{
		//keeps track of which line is currently being read
		int line_number = 1;
		
		//iterates through the lines of the file
		while ( getline (myfile,line) )
		{
			//initialize an int that will mark where the equals sign is
			int loc = 0;
			int len = line.length();
			
			//finds the location of the equals sign
			for(int i = 0; i < len; i++){
				if(line.substr(i,1) == "="){
					loc = i;
					break;
				}
			}
			
			//checks to make sure there was an equals sign
			if(loc == 0){
				cout << "Line " << line_number << " of the file did not contain an equals sign." << endl;
				line_number += 1;
				continue;
			}
			
			//split the string into twos strings: one to the left of the equals sign, one to the right
			string part1 = line.substr(0,loc);
			string part2 = line.substr(loc+1,len-loc-1);
			
			//remove unnecessary space from the two strings
			part1 = remove_end_space(part1);
			part2 = remove_beginning_space(part2);
			
			//add an entry to the unordered map
			map[part1] = part2;
			
			//increment the line number
			line_number += 1;
		}
		myfile.close();
	}
	//warns if the file was unable to be opened
	else cout << "Unable to find " << file_name << endl;
	
	//returns the map
	return map;
}

//removes spaces after the last non-space character of a string
string remove_end_space(string s){
	int len = s.length();
	while(s.substr(len-1,1) == " "){
		s = s.substr(0, len-1);
		len -= 1;
	}
	return s;
}

//removes spaces before the first non-space character of a string
string remove_beginning_space(string s){
	int len = s.length();
	while(s.substr(0,1) == " "){
		s = s.substr(1, len-1);
	}
	return s;
}

#endif // CONFIG_READER_H