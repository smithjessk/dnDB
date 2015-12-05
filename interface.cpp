#include <iostream>
#include <string>
#include <vector>
#include <sstream>

void interface();
void lexicalAnalysis(std::string c);

int main(){
	interface();
	return 0;
}

void interface(){
	std::string tempCommand;
	std::cout << "Enter Command: ";
	getline(std::cin, tempCommand);
	lexicalAnalysis(tempCommand);
}

struct commandArgs{
	std::vector<std::string> colNames;
	std::string tableName;
};

void lexicalAnalysis(std::string c){
	std::vector<std::string> commandList;
	char delimiter = ' ';
	size_t pos = 0;
	std::string token;
	std::stringstream ss(c);
	while(getline(ss, token, delimiter)){
		commandList.push_back(token);
	}
	if(commandList[0] == "CREATE"){
		if(commandList.size() == 1){
			std::cout << "Keyword CREATE requires one argument" << std::endl;
			interface();
		}else if(commandList.size() != 2){
			std::cout << "Keyword CREATE only takes one argument" << std::endl;
			interface();
		}
		if(commandList[1] == "-h"){
			std::cout << "Creates a new table with inputted name" << std::endl;
			std::cout << "Usage: CREATE table_name" << std::endl;
			interface();
		}
		std::cout << "Table Created With Name " << commandList[1] << std::endl;
		interface();
	}else if(commandList[0] == "READ"){
		if(commandList.size() == 1){
			std::cout << "Keyword READ requires one argument" << std::endl;
			interface();
		}else if(commandList.size() != 2){
			std::cout << "Keyword READ only takes one argument" << std::endl;
			interface();
		}
		if(commandList[1] == "-h"){
			std::cout << "Reads the values of a table" << std::endl;
			std::cout << "Usage: READ table_name" << std::endl;
			interface();
		}
		interface();
	}else if(commandList[0] == "ADD_COLUMN"){
		if(commandList.size() == 1 || commandList.size() == 2){
			std::cout << "Keyword ADD_COLUMN requires two arguments" << std::endl;
			interface();
		}else if(commandList.size() != 3){
			std::cout << "Keyword ADD_COLUMN only takes two arguments" << std::endl;
			interface();
		}
		if(commandList[1] == "-h"){
			std::cout << "Add a column to the table" << std::endl;
			std::cout << "Usage: ADD_COLUMN table_name col_name" << std::endl;
			interface();
		}
		interface();
	}else if(commandList[0] == "UPDATE"){
		if(commandList.size() == 1 || commandList.size() == 2 || commandList.size() == 3){
			std::cout << "Keyword UPDATE requires three arguments" << std::endl;
			interface();
		}else if(commandList.size() != 4){
			std::cout << "Keyword UPDATE only takes three arguments" << std::endl;
			interface();
		}
		if(commandList[1] == "-h"){
			std::cout << "Update the value of a cell in the table" << std::endl;
			std::cout << "Usage: UPDATE table_name row_id_no_leading_underscore col_name new_value" << std::endl;
			interface();
		}
		interface();
	}else if(commandList[0] == "DELETE"){
		if(commandList.size() == 1){
			std::cout << "Keyword DELETE requires one argument" << std::endl;
			interface();
		}else if(commandList.size() != 2){
			std::cout << "Keyword DELETE only takes one argument" << std::endl;
			interface();
		}
		if(commandList[1] == "-h"){
			std::cout << "Delete a table" << std::endl;
			std::cout << "Usage: DELETE table_name" << std::endl;
			interface();
		}
		interface();
	}else if(commandList[0] == "EXIT"){
		if(commandList[1] == "-h"){
			std::cout << "Exits the program" << std::endl;
			interface();
		}
	}else if(commandList[0] == "HELP"){
		std::cout << "Useable commands are CREATE, READ, ADD_COLUMN, UPDATE, DELETE, EXIT and HELP" << std::endl;
		std::cout << "Type -h after every command to see how it is used" << std::endl;
		interface();
	}else{
		std::cout << "Command not found, please type HELP if you need help" << std::endl;
			interface();
	}		
}