#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include "table_exception.h"

class Table {
private:
    std::string tableName = "";
    int maxID=0;
    std::unordered_map<std::string, std::unordered_map<int, std::string> > masterTable; //data structure for table
    std::vector<int> idValues;
    std::vector<std::string> colValues;
public:
    Table(std::string filepath);
    void addRowGivenID(int, std::string);
    void addRow(std::string);
    void addColumn(std::string);
    void removeRow(int);
    void removeColumn(std::string);
    void setElement(int, std::string, std::string);
    int getNumRows();
    int getNumCols();
    std::string getTableName();
    std::vector<int> getIDValues();
    std::vector<std::string> getColNames();
    std::vector<std::string> getRow(int);
    std::vector<std::string> getColumn(std::string);
    void setTableName(std::string);
    void saveTable(std::string fileName);
    std::string getSerializedTable();
};

//converts table to a single string (\n seperated lines)
//post: returns the table as a string
std::string Table::getSerializedTable() {
    std::string table = "";

    //add column row
    for (auto i = 0; i < colValues.size(); i++) {
        table += colValues[i];
        //if not the last column name, add a comma
        if (i < colValues.size() - 1) {
            table += ",";
        }
    }

    //new line character for next row
    table += "\n";

    //for each row
    for (int i : idValues) {
        std::string row = "";

        //for each element in row
        for (int j = 0; j < getRow(i).size(); j++) {
            row += getRow(i)[j];

            //if not the last element, add a comma
            if (j < getRow(i).size() - 1) {
                row += ",";
            }
        }

        //add row and new line character for next row
        table += row + "\n";
    }

    return table;
}

//checks if string is an int
bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

//returns vector of strings from a comma-separated string, left to right.
std::vector<std::string> split(std::string s){
    std::vector<std::string> result;
    std::stringstream ss(s);
    while (!ss.eof()){
        std::string x;
        getline( ss, x, ',' );
        result.push_back(x);
    }
    return result;
}

//adds a new row at with specified ID to table
//pre: addRow functions without error; CSV used by addRow was properly formatted
//post: exceptions. otherwise adds row to extracted/deduced(from maxID) ID. used
//      id is pushed to idValues. updates maxID at the end.
void Table::addRowGivenID(int id, std::string CSV){
    std::vector<std::string> rows;
    std::vector<std::string> cols = getColNames();

    if(std::find(idValues.begin(),idValues.end(),id) != idValues.end()){
        std::stringstream ss;
        ss << id;
        std::string error;
        ss >> error;
        throw TableException(error);
    }
    rows=split(CSV);

    if (rows.size() != cols.size()) {
        throw TableException("Row/column size mismatch");
    }
    idValues.push_back(id);

    int cnt=0;
    for(auto k:cols){
        masterTable[k][id]=rows[cnt];
        cnt++;
    }

    maxID = std::max(id,maxID);
    maxID++;
}

//the method thats always called first for adding a row. From CSV, figure out ID to use,
//      and call addRowGivenID
//pre: CSV properly formatted. order specified matches column order.
//post: if no ID specified in CSV, calls addRowGivenID with maxID, otherwise extracts
//      ID and calls same with found ID
void Table::addRow(std::string CSV) {
    std::vector<std::string> row = split(CSV);
    if (row.size() == 0) {
        throw TableException("Empty row");
    }

    for (int i = 0; i < row.size(); i++) {
        if (row[i].size() == 0) {
            throw TableException("Not a valid element");
        }
    }

    if (row[0].substr(0,1) == "\"") {
        std::stringstream ss;
        ss << maxID;
        std::string idString;
        ss >> idString;
        CSV = idString + "," + CSV;
        addRowGivenID(maxID, CSV);
        return;
    }
    else if(!is_number(row[0])) {
        throw TableException("Not a valid element");
        return;
    }
    else {
        int id = std::atoi(row[0].c_str());
        addRowGivenID(id, CSV);
    }
}

//add column given name to table
//pre:
//post: if colName not already in use, adds to
void Table::addColumn(std::string colName){
    masterTable[colName];

    //if column name is unique
    //add it to colValues
    //else just do nothing. SHOULD PROBABLY CHANGE THIS TO SHOW AN ERROR/WARNING TOO!~~!~!~!~~!~!~!~
    if (std::find(colValues.begin(),colValues.end(),colName) == colValues.end()) {
        colValues.push_back(colName);
    }
    else{
        throw TableException("Name "+colName+" already in use");
    }

    std::unordered_map<int,std::string> temp;
    for (int i = 0; i < idValues.size(); i++) {
        temp = {{idValues[i],colName}};
        masterTable.emplace(colName,temp);
        masterTable[colName][idValues[i]] = "\"\"";
    }
}

//iteralte through columns and erase all instances linked to given ID
//pre: id is valid int. CURRENTLY NO IMPLEMENTATION FOR IF ID IS NOT IN ID LIST~~~~~!!!!!!!~~~~
//post: removes given row. removes row ID from ID list.
void Table::removeRow(int id){
    std::vector<std::string> cols = getColNames();
    for(auto k:cols){
        masterTable[k].erase(id);
    }
    int pos = std::find(idValues.begin(),idValues.end(),id)-idValues.begin();
    idValues.erase(idValues.begin()+pos);

}

//sets element at given row and column to specified value
//pre: rowID and colID are valid. OTHERWISE SOME EXCEPTION/WARNING SHOULD BE SHOWN~!~~~~~~~~!~~~~!~!!
//post: update mastertable with the new value at desired position.
void Table::setElement(int rowID, std::string colID, std::string newValue){
    masterTable[colID][rowID]=newValue;
}

//remove column given its name
//pre: column exists. OTHERWISE SHOULD PROB Print EXCEPTION/ERROR!~~~!!!!!!!~~~~~~!!!!~~~~
//post: remove instances of colName from both table and colValues.
void Table::removeColumn(std::string colName){
    masterTable.erase(colName);
    int pos = std::find(colValues.begin(),colValues.end(),colName)-colValues.begin();
    colValues.erase(colValues.begin()+pos);
}

//returns number of rows in current table
int Table::getNumRows(){
    return idValues.size();
}

//returns number of cols in current table
int Table::getNumCols(){
    return colValues.size();
}

//returns name of table
std::string Table::getTableName() {
    return tableName;
}

//returns idValues vector.
std::vector<int> Table::getIDValues(){
    return idValues;
}

//returns colValues vector (stores column names including _id)
std::vector<std::string> Table::getColNames(){
    return colValues;
}

//return vector of row values(including "" uninitialized ones) at a specific ID
//pre: id is valid(range).. SHOULD PROBABLY DEAL WITH EXCEPTIONS ~~~~!!!!!!!!~!!!!!!!!~~~~~~
std::vector<std::string> Table::getRow(int id){
    std::vector<std::string> row;
    std::vector<std::string> cols = getColNames();
    for(auto k:cols){
        row.push_back(masterTable[k][id]);
    }
    return row;
}

//return vector of column values at specified column Name
//post; return vector
std::vector<std::string> Table::getColumn(std::string colName){
    std::vector<std::string> col;
    std::vector<int> row = getIDValues();
    for(auto k:idValues){
        col.push_back(masterTable[colName][k]);
    }
    return col;
}


//fucking constructor duh
//pre: fileName is valid path. Can be either path, or filename in same directory.
//viktor would know the pre's of this better

//post: table fill out based on file, and named to the same as file name.
Table::Table(std::string fileName) {
    std::ifstream myFile;

    myFile.open(fileName);

    if (!myFile) {
        throw TableException("No such file in directory");
    }

    int slashIndexLast = (int)fileName.find_last_of("/");

    //if there is a slash, exclude directory
    if (slashIndexLast != -1) {
        tableName = fileName.substr(slashIndexLast + 1, fileName.find(".csv") - (slashIndexLast + 1));
    }
    else {
        tableName = fileName.substr(0, fileName.find(".csv"));
    }

    std::string line;
    int lineCount = 0;
    std::string header;

    while (getline(myFile, line)) {

        //find first comma
        int comma = (int)line.find(",");

        //if header line
        if (lineCount == 0) {
            //get header string
            header = line.substr(comma + 1);

            int columnCount = 0;

            while (comma != -1) {
                comma = (int)line.find(",");

                //string before first comma
                std::string value = line.substr(0, line.find(","));

                //if first line & first column, should be "_id"
                if (columnCount == 0) {
                    if (value != "_id") {
                        //ERROR: value is not "_id"
                        throw TableException("ERROR: value '" + value + "'is not \"_id\"");
                    }
                }
                addColumn(value);

                //remove first string and comma from line for next iteration
                line = line.substr(line.find(",") + 1);
                columnCount++;
            }
        }
        //regular rows
        else {
            addRow(line);
        }

        lineCount++;
    }
}

//changes tableName to specified name
void Table::setTableName(std::string name){
    tableName=name;
}

//get table string from getSerializedTable
//pre: filepath valid
//post: table save to filepath.
void Table::saveTable(std::string fileName){
    std::ofstream save_file;
    save_file.open(fileName);

    if (!save_file.is_open()) {
        throw TableException("File did not open correctly");
    }
    save_file << getSerializedTable();

    save_file.close();
}

#endif
