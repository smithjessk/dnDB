#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

void save_table(unordered_map<string, unordered_map<int, string>> table);

int main(){
	//sample tables
	unordered_map<int, string> a{{1, "12"}, {2, "19"}, {3, "18"}};
	unordered_map<int, string> b{{1, "Peter"}, {2, "Jess"}};
	unordered_map<string, unordered_map<int, string>> hi{{"age", a}, {"name", b}};
	save_table(hi);
	return 0;
}
void save_table(unordered_map<string, unordered_map<int, string>> table){
	ofstream save_file;
	save_file.open("save.csv");
	if(save_file.is_open()){
		int counter = 0;
		vector<int> keyValue;

		//save the names of the columns 
		save_file << " ,";
		for(auto colIt = table.begin(); colIt != table.end(); ++colIt){
			save_file << "\"" << colIt->first << "\"";
			counter++;
			if(counter < table.size()){
				save_file << ",";
			}else{
				save_file << endl;
			}
			unordered_map<int, string> tempMap = colIt->second;
			for(auto rowIt = tempMap.begin(); rowIt != tempMap.end(); ++rowIt){
				keyValue.push_back(rowIt->first);
			}
		}

		sort(keyValue.begin(), keyValue.end());
		keyValue.erase(unique(keyValue.begin(), keyValue.end() ), keyValue.end());

		//save the values of each row
		counter = 0;
		int indexCounter = 0;
		int matchCounter = 0;
		while(counter < keyValue.size()){
			save_file << keyValue[indexCounter] << ",";
			for(auto colIt = table.begin(); colIt != table.end(); ++colIt){
				unordered_map<int, string> row = colIt->second;
				for(auto rowIt = row.begin(); rowIt != row.end(); ++rowIt){
					if(rowIt->first == keyValue[indexCounter]){
						save_file << "\"" << rowIt->second << "\"" << ",";
						matchCounter++;
					}	
				}
				if(matchCounter == 0){
					save_file << "\" \",";
				}
				matchCounter = 0;
			}
			
			save_file << endl;
			indexCounter++;
			counter++;
		}
	}
	save_file.close();
}