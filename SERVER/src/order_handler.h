#include <crow.h>
#include <pqxx/pqxx>
#include <string>

void CREATE_TABLE_OrdersData(pqxx::connection& conn);



void createOrders(crow::response& res, const crow::request& req, pqxx::connection& conn);


void GetOrdersId(crow::response& res, const crow::request& req, pqxx::connection& conn, const int64_t order_id);

void GetOrders(crow::response& res, const crow::request& req, pqxx::connection& conn);

void completeOrder(crow::response& res, const crow::request& req, pqxx::connection& conn);