#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

/*
take in .txt file with the format option_name = value
return an unordered_map of option_name to value
*/

std::string remove_space(std::string s); //removes all space from a string
std::unordered_map<std::string, std::string> read(std::string file_name); 
//creates a map of stuff to the left side of the equals sign to stuff to the right side of the equals sign
std::string remove_end_space(std::string s); //removes spaces after the last non-space character of a string
std::string remove_beginning_space(std::string s); //removes spaces before the first non-space character of a string

//used for testing functionality
/*int main(){
	read("example.txt");
	return 0;
}*/

//creates a map of stuff to the left side of the equals sign to stuff to the right side of the equals sign
std::unordered_map<std::string, std::string> read(std::string file_name){
	
	//intialize the map that will be returned
	std::unordered_map<std::string, std::string> map;
	
	//initialize a string for reading through the file
	std::string line;
	
	//initialize the file
	std::ifstream myfile(file_name);
	
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
				std::cout << "Line " << line_number << " of the file did not contain an equals sign." << std::endl;
				line_number += 1;
				continue;
			}
			
			//split the string into twos strings: one to the left of the equals sign, one to the right
			std::string part1 = line.substr(0,loc);
			std::string part2 = line.substr(loc+1,len-loc-1);
			
			//remove unnecessary space from the two strings
			part1 = remove_end_space(part1);
			part2 = remove_beginning_space(part2);
			
			//the next two lines can be used for testing
			//std::cout << "part1 = " << part1 << std::endl;
			//std::cout << "part2 = " << part2 << std::endl;
			
			//add an entry to the unordered map
			map[part1] = part2;
			
			//increment the line number
			line_number += 1;
		}
		myfile.close();
	}
	//warns if the file was unable to be opened
	else std::cout << "Unable to find " << file_name << std::endl;
	
	//returns the map
	return map;
}

//removes all spaces from a string, probably not going to be implemented
std::string remove_space(std::string s){
	int len = s.length();
	std::string new_string = "";
	for(int i = 0; i < len; i++){
		if(s.substr(i,1) != " "){
			new_string += s.substr(i,1);
		}
	}
	return new_string;
}

//removes spaces after the last non-space character of a string
std::string remove_end_space(std::string s){
	int len = s.length();
	while(s.substr(len-1,1) == " "){
		s = s.substr(0, len-1);
		len -= 1;
	}
	return s;
}

//removes spaces before the first non-space character of a string
std::string remove_beginning_space(std::string s){
	int len = s.length();
	while(s.substr(0,1) == " "){
		s = s.substr(1, len-1);
	}
	return s;
}
