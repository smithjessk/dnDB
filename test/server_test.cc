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
  to_send["col_name"] = "name";
  to_send["row_id"] = 1;
  to_send["value"] = "ictorVay";
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

void create_table(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name;
  std::string data = to_send.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/create");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n");
}

void add_column(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name;
  to_send["col_name"] = "occupation";
  std::string data = to_send.toStyledString();
    CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, 
    "http://localhost:8080/table/add_column");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n"); 
}

void add_row(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name;
  to_send["row"] = "\"Ali\",\"1000\",\"orange\"";
  std::string data = to_send.toStyledString();
    CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, 
    "http://localhost:8080/table/add_row");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n"); 
}

void sanity_test(std::string table_name) {
  std::thread t1(read_table, table_name);
  t1.join();
  std::thread t2(update_table, table_name);
  t2.join();
  std::thread t3(read_table, table_name);
  t3.join();
  std::thread t4(add_row, table_name);
  t4.join();
  std::thread t5(add_column, table_name);
  t5.join();
  std::thread t6(read_table, table_name);
  t6.join();
}

void quote_test(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name;
  to_send["row"] = "\"Ali\",\"1000\",\"orange\"";
  std::string data = to_send.toStyledString();
    CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, 
    "http://localhost:8080/table/add_row");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n"); 
}

void invalid_server_name_test(std::string table_name) {
  Json::Value to_send;
  to_send["table_name"] = table_name + "blablabla";
  std::string data = to_send.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/read");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n");
}

void delete_read_test(std::string table_name) {
  delete_table(table_name);
  read_table(table_name);
}

void create_test(std::string table_name) {
  create_table(table_name);
  read_table(table_name);
}

void multiple_tables_test(std::string name_a, std::string name_b) {
  create_test(name_b);
  std::thread t1(read_table, name_a);
  std::thread t2(read_table, name_b);
  t1.join();
  t2.join();
}

void sql_test(std::string table_name, std::vector<std::string> col_names) {
  std::string statement = "SELECT ";
  for (size_t i = 0; i < col_names.size() - 1; i++) {
    statement += col_names.at(i) + ",";
  }
  statement += col_names.at(col_names.size() - 1) + " FROM " + table_name;
  Json::Value to_send;
  to_send["table_name"] = table_name;
  to_send["statement"] = statement;
  std::string data = to_send.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/sql");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_perform(curl);
  std::printf("\n");
}

int main() {
  // sanity_test("sample");
  // invalid_server_name_test("sample");
  // delete_read_test("sample");
  // create_test("sampleA");
  // multiple_tables_test("sample", "sampleA");
  std::vector<std::string> col_names = {"\"name\"", "\"age\""};
  sql_test("sample", col_names);
  return 0;
}