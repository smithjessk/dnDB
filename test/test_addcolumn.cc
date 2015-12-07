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

int main(){
    Table table("../data/sample.csv");

    std::cout << "Table name: " << table.getTableName() << std::endl;
    std::cout << std::endl;

    std::cout << "Original: " << std::endl;
    std::cout << table.getSerializedTable() << std::endl;

    //adding empty string as column name
    try {
        std::cout << "Adding column with empty string..." << std::endl;
        table.addColumn("");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    std::cout << std::endl;

    //adding a column normally
    try {
        std::cout << "Adding column with specific name..." << std::endl;
        table.addColumn("\"Last Name\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    std::cout << std::endl;

    //adding the same column name
    try {
        std::cout << "Adding another column with same name..." << std::endl;
        table.addColumn("\"Last Name\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    std::cout << std::endl;

    //getting column
    try {
        std::cout << "Getting column with specified name..." << std::endl;
        print(table.getColumn("\"Last Name\""));
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    std::cout << std::endl;

    //removing column that exists
    try {
        std::cout << "Removing column with specific name..." << std::endl;
        table.removeColumn("\"Last Name\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    std::cout << std::endl;

    //removing column that doesnt exist
    try {
        std::cout << "Removing column with specific name that is not in table..." << std::endl;
        table.removeColumn("\"Last Name\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    std::cout << std::endl;

    //getting column that doesnt exist
    try {
        std::cout << "Getting column with specified name that is not in table..." << std::endl;
        print(table.getColumn("\"Last Name\""));
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    std::cout << std::endl;

}