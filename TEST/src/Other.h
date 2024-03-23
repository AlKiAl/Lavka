#include <curl/curl.h>
#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

int GET(const std::string& url, std::string& buffer);

void DELETE_TABLE_BY_NAME(pqxx::connection& conn, const std::string& table_name);

int POST(const std::string& url, const std::string& jsonData, std::string& buffer);