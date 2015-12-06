#include <iostream>
#include "table.h"

int main() {
    
    Table table1, table2;
    
    std::cout << "\n===table1, using default constructor===\n" << std::endl;
    
    try {
        std::cout << "Table name: " << table1.getTableName() << std::endl;
        std::cout << std::endl;
        
        std::cout << "Original: " << std::endl;
        std::cout << table1.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    
    try {
        std::cout << "Adding column \"name\"..." << std::endl;
        table1.addColumn("\"name\"");
        std::cout << table1.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    
    try {
        std::cout << "Saving table1 into save.csv..." << std::endl;
        table1.saveTable("./data/save.csv");
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    
    std::cout << "\n===table2, using constructor on blank file===\n" << std::endl;
    
    try {
        table2 = *new Table("./data/blank.csv");
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    
    try {
        std::cout << "Table name: " << table2.getTableName() << std::endl;
        std::cout << std::endl;
        
        std::cout << "Original: " << std::endl;
        std::cout << table2.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    
    try {
        std::cout << "Adding column \"name\"..." << std::endl;
        table2.addColumn("\"name\"");
        std::cout << table2.getSerializedTable() << std::endl;
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
    
    try {
        std::cout << "Saving table2 into save2.csv..." << std::endl;
        table2.saveTable("./data/save2.csv");
    }
    catch (TableException &cException) {
        std::cerr << "A table exception occurred (" << cException.getError() << ")\n" << std::endl;
    }
}
