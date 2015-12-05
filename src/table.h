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
        std::vector<std::string> getColNames();
        std::vector<std::string> getRow(int);
        void save_table(std::string fileName);
};

//prints string vector
void print(std::vector<std::string> v){
    for(auto i=0;i<v.size();i++){
        std::cout<<v[i];
        if(i<v.size()-1){
            std::cout<<",";
        }
    }
    std::cout<<std::endl;
}

//prints int vector
void print(std::vector<int> v){
    for(auto i=0;i<v.size();i++){
        std::cout<<v[i];
        if(i<v.size()-1){
            std::cout<<",";
        }
    }
    std::cout<<std::endl;
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

void Table::addRowGivenID(int id, std::string CSV){
    std::vector<std::string> rows;
    std::vector<std::string> cols = getColNames();

    rows=split(CSV);

    //FOO MUST BE SORTED BY COLS (Corresponding column to parsed value aka name:"jess", age:"14")
    //THROW EXCEPTION IF SIZE MISMATCH

    if (rows.size() != cols.size()) {
        std::cout<< "Row/column size mismatch"<<std::endl;
        return;
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

void Table::addRow(std::string CSV){
    std::vector<std::string> row = split(CSV);
    if(row.size()==0){
        std::cout<<"error1";
        return;
    }
    if(row[0].size()==0){
        std::cout<<"error2";
        return;
    }
    if(row[0].substr(0,1)=="\""){
        std::stringstream ss;
        ss << maxID;
        std::string idString;
        ss >> idString;
        CSV=idString+","+CSV;
        addRowGivenID(maxID,CSV);
        return;
    }else if(!is_number(row[0])){
        std::cout<<"error3";
        return;
    }else{
        int id = std::atoi(row[0].c_str());
        addRowGivenID(id,CSV);
    }
}

void Table::addColumn(std::string colName){
    masterTable[colName];

    //if column name is unique
    if (std::find(colValues.begin(),colValues.end(),colName) == colValues.end()) {
        colValues.push_back(colName);
    }
}

void Table::removeRow(int id){
    std::vector<std::string> cols = getColNames();
    for(auto k:cols){
        masterTable[k].erase(id);
    }
    int pos = std::find(idValues.begin(),idValues.end(),id)-idValues.begin();
    idValues.erase(idValues.begin()+pos);

}

void Table::setElement(int rowID, std::string colID, std::string newValue){
    masterTable[colID][rowID]=newValue;
}

void Table::removeColumn(std::string colName){
    masterTable.erase(colName);
    int pos = std::find(colValues.begin(),colValues.end(),colName)-colValues.begin();
    colValues.erase(colValues.begin()+pos);
}

int Table::getNumRows(){
    return idValues.size();
}

int Table::getNumCols(){
    return colValues.size();
}

std::string Table::getTableName() {
    return tableName;
}

std::vector<std::string> Table::getColNames(){
    return colValues;
}

std::vector<std::string> Table::getRow(int id){
    std::vector<std::string> row;
    std::vector<std::string> cols = getColNames();
    for(auto k:cols){
        row.push_back(masterTable[k][id]);
    }
    return row;
}

Table::Table(std::string fileName) {
    std::ifstream myFile;

    myFile.open(fileName);

    if (!myFile) {
        throw "No such file in directory.";
    }

    tableName = fileName.substr(0, fileName.find(".csv"));

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
                        std:: string error = "ERROR: value '" + value + "'is not \"_id\"";
                        throw error;
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

void Table::save_table(std::string fileName){
    std::ofstream save_file;
    save_file.open(fileName);

    if (!save_file.is_open()) {
        throw "File did not open correctly.";
    }

    int counter = 0;
    std::vector<int> keyValue;

    for(auto colIt = colValues.begin(); colIt != colValues.end(); ++colIt){
        save_file << *colIt;
        counter++;
        if(counter < masterTable.size()){
            save_file << ",";
        }else{
            save_file << std::endl;
        }
    }

    //save the values of each row
    counter = 0;
    int indexCounter = 0;
    int matchCounter = 0;
    while(counter < idValues.size()){
        std::vector<std::string> rowVector;
        for(auto k=idValues.begin();k!=idValues.end();++k){
            rowVector=getRow(*k);

            for(int j=0;j<rowVector.size();j++){
                std::string curr = rowVector[j];
                if(curr.size()==0){
                    save_file << "\"\"";
                    if(j < rowVector.size()-1){
                        save_file << ",";
                    }
                    continue;
                }
                save_file << curr;
                //add the comma to separate entries
                if(j < rowVector.size()-1){
                    save_file << ",";
                }
                matchCounter = 0;
            }

            //new line for new id
            save_file << std::endl;
            indexCounter++;
            counter++;


        }
    }

    save_file.close();
}

#endif // TABLE_H