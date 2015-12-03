#include <cstdio>
#include <curl/curl.h>
#include <thread>

#include "../deps/json/json.h"

std::vector<std::string> table_names;

void read_table(std::string table_name) {
  Json::Value root;
  root["table_name"] = table_name;
  std::string s = root.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/read");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, s.c_str());
  curl_easy_perform(curl);
  std::printf("\n");
}

int main() {
  table_names.push_back("one");
  table_names.push_back("two");
  std::thread t1(read_table, "one");
  std::thread t2(read_table, "two");
  t1.join();
  t2.join();
  return 0;
}