//table class
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

class Table {
    public:
        int numRows, numCols;
        std::unordered_map<std::string, std::unordered_map<int, std::string> > masterTable;
        Table(std::string filepath);
        void addElement(std::vector<std::string>); //maybe different argument
        void dropColumn(std::string columnName);
        void dropRow(std::string rowName);
        int getNumRows();
        int getNumCols();
        std::vector<std::string> getColNames();
};

Table::Table(std::string filepath){ //CSVreader to masterTable
    
    //initialize numRows, numCols
}

void Table::addElement(std::vector<std::string>){ 
  
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

