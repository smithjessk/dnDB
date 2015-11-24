#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

void save_table(unordered_map<string, unordered_map<int, string>> table);

int main(){
	unordered_map<int, string> a{{1, "12"}, {2, "19"}};
	unordered_map<int, string> b{{1, "Peter"}, {2, "Jess"}};
	unordered_map<string, unordered_map<int, string>> hi{{"age", a}, {"name", b}};
	save_table(hi);
	return 0;
}
void save_table(unordered_map<string, unordered_map<int, string>> table){
	vector<vector<string>> values;
	vector<string> colNames;

	for(auto colIt = table.begin(); colIt != table.end(); ++colIt){
		colNames.push_back("\"" + colIt->first + "\"");
	}
	values.push_back(colNames);

	for(auto colIt = table.begin(); colIt != table.end(); ++colIt){
		unordered_map<int, string> row = colIt->second;
		for(auto rowIt = row.begin(); rowIt != row.end(); ++rowIt){
		}
	}
	ofstream save_file;
	save_file.open("save.csv");
	if(save_file.is_open()){
		for(vector<vector<string>>::iterator it = values.begin(); it != values.end(); ++it){
		vector<string> temp = *it;
		for(int j = 0; j < temp.size(); j++){
			if(j != temp.size() - 1){
				save_file << temp[j] << ",";
			}else{
				save_file << temp[j];
			}
		}
	}
	}
	
}