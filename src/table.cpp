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
        bool is_number(const std::string);
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

bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
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

Table(std::string fileName) {
    std::ifstream myFile;
    myFile.open(fileName);
    if (!myFile) {
        return -1;
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
                        std::cout << "ERROR: value '" << value << "'is not \"_id\"" << std::endl;
                        return -1;
                    }
                }
                //IMPLEMENT: addColumn(value);
                
                //remove first string and comma from line for next iteration
                line = line.substr(line.find(",") + 1);
                columnCount++;
            }
        }
        //regular rows
        else {
            std::string id = line.substr(0, comma);
            if (!is_number(id)) {
                std::cout << "ERROR: id " << id << " not an int" << std::endl;
                return -1;
            }
            
            int value = std::stoi(id);
            
            //IMPLEMENT: addRow(value ,header, line.substr(comma + 1));
        }
        
        lineCount++;
    }
    
    return 0;
}

