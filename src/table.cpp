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
        void addRow(std::string);
        void addRow(int, std::string);
        void addColumn(std::string);
        void removeRow(int);
        void removeColumn(std::string);
        int getNumRows();
        int getNumCols();
        std::vector<std::string> getColNames();
};

Table::Table(std::string filepath){ //CSVreader to masterTable
    
    //initialize numRows, numCols
}

void Table::addRow(std::string CSV){ //
    //GENERATE id, 
    //call addRow(int, string);
}

void Table::addRow(int id, std::string CSV){ //
    //foo stores parsed CSV

    std::vector<std::string> foo {"rowval1", "rowval2", "rowval3"}; //TEMPORARY. SHOULD BE DERIVED FROM CSVREADER
    std::vector<std::string> cols = getColNames();
    //FOO MUST BE SORTED BY COLS (Corresponding column to parsed value aka name:"jess", age:"14")
    //THROW EXCEPTION IF SIZE MISMATCH

    int cnt=0;
    for(auto k:cols){
        masterTable[k][id]=foo.at(cnt);
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

