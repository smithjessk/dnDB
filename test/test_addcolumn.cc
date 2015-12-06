#include <iostream>
#include "table.h"

int main(){
    Table table("table.csv");

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


    //adding a column normally
    try {
        std::cout << "Adding column with specific name..." << std::endl;
        table.addColumn("\"Last Name\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }

    //adding the same column name
    try {
        std::cout << "Adding another column with same name..." << std::endl;
        table.addColumn("\"Last Name\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
}
