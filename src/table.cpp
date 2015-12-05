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
    public:
        int numRows=0, numCols=0, maxID=0;      //numRows does NOT account for header row.
        std::unordered_map<std::string, std::unordered_map<int, std::string> > masterTable;
        std::vector<int> idValues;
        Table(std::string filepath);
        void addRowGivenID(int, std::string);
        void addRow(std::string);
        void addColumn(std::string);
        void removeRow(int);
        void removeColumn(std::string);
        int getNumRows();
        int getNumCols();
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

void Table::addRowGivenID(int id, std::string CSV){
    std::vector<std::string> rows;
    std::vector<std::string> cols = getColNames();
    std::cout<<CSV<<std::endl;

    rows=split(CSV);

    //FOO MUST BE SORTED BY COLS (Corresponding column to parsed value aka name:"jess", age:"14")
    //THROW EXCEPTION IF SIZE MISMATCH

    if (rows.size() != cols.size()) {
        std::cout<< "Row/column size mismatch"<<std::endl;
        return;
    }
    idValues.push_back(id);
    //std::reverse(cols.begin(),cols.end());

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

    /*std::unordered_map<int,std::string> temp;
    for(int i=0;i<idValues.size();i++){             //LOOK
        temp={{idValues[i],colName}};               //HERE
        //masterTable.emplace(colName,temp);        //!!!!!!!!
        masterTable[colName][idValues[i]]=colName;  //CHANGE THIS TO SAY MAYBE "EMPTY" OR "NOT SPECIFIED" OR SOME SHIT IF NEEDED
    }
    */
}

void Table::removeRow(int id){
    std::vector<std::string> cols = getColNames();
    for(auto k:cols){
        masterTable[k].erase(id);
    }
    int pos = std::find(idValues.begin(),idValues.end(),id)-idValues.begin();
    idValues.erase(idValues.begin()+pos);
    numRows--;
}

void Table::removeColumn(std::string colName){
    masterTable.erase(colName);
    numCols--;
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
    std::reverse(keys.begin(),keys.end());
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
            /*
            std::string id = line.substr(0, comma);
            if (!is_number(id)) {
                std::string error = "ERROR: id " + id + " not an int";
                throw error;
            }
            int value;
            std::stringstream ss;
            ss<<id;
            ss>>value;
            //addRowGivenID(value, line);
            */
            addRow(line);
            //std::cout<<value<<std::endl;
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
    //added this so i can read to it from the mastertable, so that saving header line to file is in right order.
    std::vector<std::string> colValues = getColNames();

    //save the names of the columns
    //save_file << "_id,";
    /*
    for(auto colIt = masterTable.begin(); colIt != masterTable.end(); ++colIt){
        colValues.push_back(colIt->first);

        //compile list of all the ids possible in the map
        //std::unordered_map<int, std::string> tempMap = colIt->second;
        /*for(auto rowIt = tempMap.begin(); rowIt != tempMap.end(); ++rowIt){
            keyValue.push_back(rowIt->first);
        }
    }
    */


    for(auto colIt = colValues.begin(); colIt != colValues.end(); ++colIt){
        save_file << *colIt;
        counter++;
        if(counter < masterTable.size()){
            save_file << ",";
        }else{
            save_file << std::endl;
        }
    }
    //make the values unique
    //sort(idValues.begin(), idValues.end());
    //idValues.erase(unique(idValues.begin(), idValues.end() ), idValues.end());

    std::reverse(colValues.begin(),colValues.end());
    //std::reverse(idValues.begin(),idValues.end());
    //remove cuz using this would reverse the row order.

    //save the values of each row
    counter = 0;
    int indexCounter = 0;
    int matchCounter = 0;
    while(counter < idValues.size()){
        std::vector<std::string> rowVector;
        for(auto k=idValues.begin();k!=idValues.end();++k){
            rowVector=getRow(*k);
            //std::reverse(rowVector.begin(),rowVector.end());

            for(int j=0;j<rowVector.size();j++){
                std::string curr = rowVector[j];
                if(curr.size()==0){
                    save_file << "\"\"";             //!!!!!!!!!!!!! HERE !!!!!!!!!!! change the string to whatever else as desired.
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
        /*for(auto rowIt = row.begin(); rowIt != row.end(); ++rowIt){
            if(rowIt->first == idValues[indexCounter]){
                save_file << rowIt->second;
                matchCounter++;
            }

        }*/
        //adds a space for empty values

    }
    save_file.close();
}
