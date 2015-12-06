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
    Table table("table.csv");                               //constructor test

    //table.addRowGivenID(5,"5,\"James\",\"16\",y");        //addRowGivenID test

    table.addRow("77,\"James\",\"16\",\"none\"");         //regular addRow test
    table.addRow("\"Kevin\",\"44\",\"fries\"");
/*
    table.addColumn("New Column22");                        //addColumn test(s)
    table.addColumn("c1");
*/
    table.addColumn("\"New Column22\"");
    table.addColumn("\"Last Name\"");
    table.addColumn("\"New Column\"");
    table.addRow("\"Jack\",\"54\",\"cucumber\",\"22\",\"Smith\",\"cc\"");
    table.addRow("\"Chad\",\"69\",\"banana\",\"22\",\"Caesar\",\"cc\"");

    table.setElement(4,"\"Last Name\"", "\"Azari\"");
    //table.addColumn("z");
    print(table.getRow(4));                                 //getRow test
    print(table.getColumn("\"food\""));                     //getColumn test

    //std::cout<<table.getSerializedTable()<<std::endl;     //table test

    table.saveTable("save.csv");                            //save test

    return 0;
}
