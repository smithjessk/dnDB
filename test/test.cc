#include <iostream>

#include "../deps/json/json.h"

int main() {
  Json::Value root;
  root["abc"] = "def";
  Json::StreamWriterBuilder builder;
  Json::StreamWriter *writer(builder.newStreamWriter());
  writer->write(root, &std::cout);
  std::cout << std::endl;
  return 0;
}