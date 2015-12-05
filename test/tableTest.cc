#include "table.h"

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
    Table table("table.csv");
    //table.addColumn("\"newColumn\"");
    //table.addRow(5,"5,\"James\",\"16\",y");
    //table.addColumn("\"name\"");
    table.addRow("77,\"James\",\"16\",\"none\"");
    table.addRow("\"Kevin\",\"44\",\"fries\"");
    //table.addColumn("New Column22");
    table.addColumn("New Column22");
    //table.addColumn("c1");
    table.addColumn("Last Name");
    table.addColumn("New Column");
    table.addRow("\"Jack\",\"54\",\"cucumber\",22,\"Smith\",cc");
    table.addRow("\"Chad\",\"69\",\"banana\",22,\"Caesar\",cc");

    table.setElement(4,"Last Name", "Azari");
    //table.addColumn("z");
    print(table.getRow(4));


    table.saveTable("save.csv");
    return 0;
}
