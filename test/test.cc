#include <cstdio>
#include <curl/curl.h>

#include "../deps/json/json.h"

int main() {
  Json::Value root;
  root["table_name"] = "one";
  std::string s = root.toStyledString();
  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/table/read");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, s.c_str());
  curl_easy_perform(curl);
  std::printf("\n");
  return 0;
}