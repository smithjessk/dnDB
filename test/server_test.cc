#include <cstdio>
#include <curl/curl.h>
#include <thread>
#include <vector>

#include "../deps/json/json.h"

void read_table(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name;
  std::string data = to_send.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/read");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n");
}

void update_table(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name;
  to_send["col_name"] = "sample_col_name";
  to_send["row_id"] = 1;
  to_send["value"] = "new_val";
  std::string data = to_send.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/update");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n");
}

void delete_table(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name;
  std::string data = to_send.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/delete");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n"); 
}

int main() {
  int num_threads = 6;
  std::vector<std::string> table_names = {"sample"};
  std::thread t1(read_table, "./data/sample");
  t1.join();
  return 0;
}