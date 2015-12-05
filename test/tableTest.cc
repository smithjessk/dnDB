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
    table.addRow("\"Chad\",\"69\",\"banana\",22,\"Thundercock\",cc");


    //table.removeColumn("Last Name");
    //std::cout<<table.masterTable["New Column22"][79];
    //table.addColumn("z");
    //print(table.getRow(5));

    table.setElement(4,"Last Name", "Azari");
    table.save_table("save.csv");
    return 0;
}
