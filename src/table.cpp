//table class
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

class Table {
    public:
        int numRows, numColumns;
        std::unordered_map<std::string, std::unordered_map<int, std::string> > masterTable;
        //Table(std::string filepath);
        Table(int,int);
        void addElement(std::vector<std::string>);
        //drop(std::string columnName);
        int getNumRows();
        int getNumCols();
        std::vector<std::string> getColNames();
};

Table::Table(int r, int c){
    numRows=r;
    numColumns=c;
}

void Table::addElement(std::vector<std::string>){
  
}

int Table::getNumRows(){
    return numRows;
}

int Table::getNumCols(){
    return numColumns;
}

std::vector<std::string> Table::getColNames(){
    std::vector<std::string> keys;
    for(auto k: masterTable){   
        keys.push_back(k.first);
    }
    return keys;

}


int main()
{

    return 0;
}
