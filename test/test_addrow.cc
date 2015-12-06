#include <iostream>
#include "table.h"

int main(){
    Table table("./data/table.csv");
    
    std::cout << "Table name: " << table.getTableName() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Original: " << std::endl;
    std::cout << table.getSerializedTable() << std::endl;
    
    try {
        std::cout << "Adding row with empty string..." << std::endl;
        table.addRow("");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    
    try {
        std::cout << "Adding row with no given ID..." << std::endl;
        table.addRow("\"Jack\",\"54\",\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    
    try {
        std::cout << "Adding same row with custom ID 6..." << std::endl;
        table.addRow("6,\"Jack\",\"54\",\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
    
    try {
        std::cout << "Adding same row with custom ID 6..." << std::endl;
        table.addRow("6,\"Jack\",\"54\",\"cucumber\"");
        std::cout << table.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")" << std::endl;
    }
}
