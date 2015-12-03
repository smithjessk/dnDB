#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

void save_table(unordered_map<string, unordered_map<int, string>> table);

void save_table(){
	ofstream save_file;
	save_file.open("save.csv");
	if(save_file.is_open()){
		int counter = 0;
		vector<int> keyValue;

		//save the names of the columns 
		save_file << "_id,";
		for(auto colIt = masterTable.begin(); colIt != masterTable.end(); ++colIt){
			save_file << "\"" << colIt->first << "\"";
			counter++;
			if(counter < masterTable.size()){
				save_file << ",";
			}else{
				save_file << endl;
			}
			//compile list of all the ids possible in the map
			unordered_map<int, string> tempMap = colIt->second;
			for(auto rowIt = tempMap.begin(); rowIt != tempMap.end(); ++rowIt){
				keyValue.push_back(rowIt->first);
			}
		}

		//make the values unique
		sort(keyValue.begin(), keyValue.end());
		keyValue.erase(unique(keyValue.begin(), keyValue.end() ), keyValue.end());

		//save the values of each row
		counter = 0;
		int indexCounter = 0;
		int matchCounter = 0;
		while(counter < keyValue.size()){
			save_file << keyValue[indexCounter] << ",";
			for(auto colIt = masterTable.begin(); colIt != masterTable.end(); ++colIt){
				unordered_map<int, string> row = colIt->second;
				for(auto rowIt = row.begin(); rowIt != row.end(); ++rowIt){
					if(rowIt->first == keyValue[indexCounter]){
						save_file << "\"" << rowIt->second << "\"";
						matchCounter++;
					}
				
}				//adds a space for empty values
				if(matchCounter == 0){
					save_file << "\" \"";
				}
				//add the comma to separate entries
				if(colIt != --masterTable.end()){
					save_file << ",";
				}
				matchCounter = 0;
			}
			//new line for new id
			save_file << endl;
			indexCounter++;
			counter++;
		}
	}
	save_file.close();
}