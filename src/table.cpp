//table class
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>

class Table {
    public:
        int numRows, numCols;
        std::unordered_map<std::string, std::unordered_map<int, std::string> > masterTable;
        Table(std::string filepath);
        //void addRow(std::string); do we need?
        void addRow(int, std::string);
        void addColumn(std::string);
        void removeRow(int);
        void removeColumn(std::string);
        int getNumRows();
        int getNumCols();
        std::vector<std::string> getColNames();
        std::vector<std::string> getRow(int);
};

bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

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

void Table::addRow(int id, std::string CSV){
    std::vector<std::string> rows;
    std::vector<std::string> cols = getColNames();
    rows=split(CSV);

    //FOO MUST BE SORTED BY COLS (Corresponding column to parsed value aka name:"jess", age:"14")
    //THROW EXCEPTION IF SIZE MISMATCH

    int cnt=0;
    for(auto k:cols){
        masterTable[k][id]=rows.at(cnt);
        cnt++;
    }

}

void Table::addColumn(std::string colName){
    masterTable[colName];
}

void Table::removeRow(int id){
    std::vector<std::string> cols = getColNames();
    for(auto k:cols){
        masterTable[k].erase(id);
    }
}

void Table::removeColumn(std::string colName){
    masterTable.erase(colName);
}

int Table::getNumRows(){
    return numRows;
}

int Table::getNumCols(){
    return numCols;
}

std::vector<std::string> Table::getColNames(){
    std::vector<std::string> keys;
    for(auto k: masterTable){
        keys.push_back(k.first);
    }
    return keys;

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
                //std::cout << "value: " << value << std::endl;

                //remove first string and comma from line for next iteration
                line = line.substr(line.find(",") + 1);
                columnCount++;
            }
        }
        //regular rows
        else {
            std::string id = line.substr(0, comma);
            if (!is_number(id)) {
                std::string error = "ERROR: id " + id + " not an int";
                throw error;
            }

            int value;
            std::stringstream ss;
            ss<<id;
            ss>>value;
            addRow(value, line);
            //std::cout<<value<<std::endl;
        }

        lineCount++;
    }
}


int main(){
    Table table("table.csv");
    std::vector<std::string> rows = table.getRow(2);
    for(std::string s:rows){
        std::cout<<s<<" ";
    }



    return 0;

}
