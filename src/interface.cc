#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <curl/curl.h>

#include "../deps/json/json.h"

void interface();
void lexicalAnalysis(std::string c);
void create(std::vector<std::string> command);
void read(std::vector<std::string> command);
void addColumn(std::vector<std::string> command);
void update(std::vector<std::string> command);
void deleteTable(std::vector<std::string> command);

std::vector<std::string> commandList;

int main(){
	interface();
	return 0;
}

void interface() {
	std::string tempCommand;
	std::cout << "Enter Command: ";
	getline(std::cin, tempCommand);
	commandList = std::vector<std::string> (0);
	lexicalAnalysis(tempCommand);
}

struct commandArgs{
	std::vector<std::string> colNames;
	std::string tableName;
};

//check the commands
void lexicalAnalysis(std::string c){
	char delimiter = ' ';
	std::string token;
	std::stringstream ss(c);
	while(getline(ss, token, delimiter)){
		commandList.push_back(token);
	}
	//check create command
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
		create(commandList);
	}
	//check read command
	else if(commandList[0] == "READ"){
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
		read(commandList);
	}
	//check the add column command
	else if(commandList[0] == "ADD_COLUMN"){
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
		addColumn(commandList);
	}
	//check update command
	else if(commandList[0] == "UPDATE"){
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
		update(commandList);
	}
	//check delete command
	else if(commandList[0] == "DELETE"){
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
		deleteTable(commandList);
	}
	//Exit the program
	else if(commandList[0] == "EXIT"){
		if(commandList[1] == "-h"){
			std::cout << "Exits the program" << std::endl;
			interface();
		}
		std::exit(0);
	}
	//Help
	else if(commandList[0] == "HELP"){
		std::cout << "Useable commands are CREATE, READ, ADD_COLUMN, UPDATE, DELETE, EXIT and HELP" << std::endl;
		std::cout << "Type -h after every command to see how it is used" << std::endl;
		interface();
	}
	//Other commands not recognized
	else{
		std::cout << "Command not found, please type HELP if you need help" << 
			std::endl;
		interface();
	}		
}

//server requests
//create the table
void create(std::vector<std::string> command){
	Json::Value to_send;
  	to_send["table_name"] = commandList[1];
 	std::string data = to_send.toStyledString();
  	CURL *curl = curl_easy_init();
 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/create");
  	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
 	curl_easy_perform(curl);
	std::printf("\n");
}

//read table
void read(std::vector<std::string> command){
	Json::Value to_send;
  	to_send["table_name"] = commandList[1];
 	std::string data = to_send.toStyledString();
  	CURL *curl = curl_easy_init();
 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/read");
  	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
 	curl_easy_perform(curl);
	std::printf("\n");
}

//add column
void addColumn(std::vector<std::string> command){
	Json::Value to_send;
  	to_send["table_name"] = commandList[1];
  	to_send["col_name"] = commandList[2];
 	std::string data = to_send.toStyledString();
  	CURL *curl = curl_easy_init();
 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/add_column");
  	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
 	curl_easy_perform(curl);
	std::printf("\n");
}

//update value
void update(std::vector<std::string> command){
	Json::Value to_send;
  	to_send["table_name"] = commandList[1];
  	to_send["row_id"] = commandList[2];
  	to_send["col_name"] = commandList[3];
  	to_send["value"] = commandList[4];
 	std::string data = to_send.toStyledString();
  	CURL *curl = curl_easy_init();
 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/update");
  	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
 	curl_easy_perform(curl);
	std::printf("\n");
}

//delete table
void deleteTable(std::vector<std::string> command){
	Json::Value to_send;
  	to_send["table_name"] = commandList[1];
 	std::string data = to_send.toStyledString();
  	CURL *curl = curl_easy_init();
 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/delete");
  	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
 	curl_easy_perform(curl);
	std::printf("\n");
}

void sql(std::vector<std::string> command){
	Json::Value to_send;
  	to_send["table_name"] = commandList[1];
 	std::string data = to_send.toStyledString();
  	CURL *curl = curl_easy_init();
 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/sql");
  	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
 	curl_easy_perform(curl);
	std::printf("\n");
}