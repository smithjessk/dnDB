#include <iostream>
#include "../src/table.h"

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

int main() {

    Table table = *new Table("../data/sample.csv");

    try {
        std::cout << "Table name: " << table.getTableName() << std::endl;
        std::cout << std::endl;

        std::cout << "Original: " << std::endl;
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //add row with empty string
    try {
        std::cout << "Adding row with empty string..." << std::endl;
        table.addRow("");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //add row with one extra element
    try {
        std::cout << "Adding row one extra element..." << std::endl;
        table.addRow("\"Jack\",\"54\",\"cucumber\",\"extra column\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //add row with an (invalid) empty element
    try {
        std::cout << "Adding row an empty element..." << std::endl;
        table.addRow("\"Jack\",,\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //add row with no given ID
    try {
        std::cout << "Adding row with no given ID..." << std::endl;
        table.addRow("\"Jack\",\"54\",\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //add row with custom ID 6
    try {
        std::cout << "Adding same row with custom ID 6..." << std::endl;
        table.addRow("6,\"Jack\",\"54\",\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //add row with custom ID 6, should throw exception
    try {
        std::cout << "Adding same row with custom ID 6..." << std::endl;
        table.addRow("6,\"Jack\",\"54\",\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //get row with id 5
    try {
        std::cout << "Getting row with id 5..." << std::endl;
        print(table.getRow(5));
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //remove row with ID 5
    try {
        std::cout << "Remove row with ID 5..." << std::endl;
        table.removeRow(5);
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //get row with id 5 (doesnt exist)
    try {
        std::cout << "Getting row with id 5 (which should be already removed)..." << std::endl;
        print(table.getRow(5));
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;


    //add row with no given ID, ID should be 7
    try {
        std::cout << "Adding row with no given ID..." << std::endl;
        table.addRow("\"Jack\",\"54\",\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //print row with ID 3
    try {
        std::cout << "Get row with ID 3..." << std::endl;
        for (int i = 0; i < table.getRow(3).size(); i++) {
            std::cout << table.getRow(3)[i];
            if (i < table.getRow(3).size() - 1) {
                 std::cout << ", ";
            }
        }
        std::cout << "\n" << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //print row with ID 100
    try {
        std::cout << "Get row with ID 100..." << std::endl;
        for (int i = 0; i < table.getRow(100).size(); i++) {
            std::cout << table.getRow(100)[i];
            if (i < table.getRow(100).size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "\n" << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //remove row with ID 100
    try {
        std::cout << "Remove row with ID 100..." << std::endl;
        table.removeRow(100);
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    try {
        std::cout << "Saving table into save.csv..." << std::endl;
        table.saveTable("save.csv");
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
}