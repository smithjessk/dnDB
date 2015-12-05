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
  std::vector<std::string> table_names = {"one", "two"};
  std::vector<std::thread> threads;
  for (size_t i = 0; i < num_threads; i++) {
    size_t table_name_index = i % table_names.size();
    size_t quotient = i / 2;
    switch (quotient) {
      case 0: {
        threads.push_back(std::thread(read_table, 
          table_names.at(table_name_index)));  
        break;
      }
      case 1: {
        threads.push_back(std::thread(update_table,
          table_names.at(table_name_index)));
        break;
      }
      case 2: {
        threads.push_back(std::thread(delete_table,
          table_names.at(table_name_index)));
        break; 
      }
      default: {
        std::printf("Got unexpected quotient %zu\n", table_name_index);
        break;
      }
    }
  }
  for (size_t i = 0; i < threads.size(); i++) {
    threads.at(i).join();
  }
  return 0;
}