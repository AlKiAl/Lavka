#include <nlohmann/json.hpp>
#include <iostream>
#include "courier_handler.h"
#include "order_handler.h"
#include "RateLimit.h"


using namespace std;
using json = nlohmann::json;


int main() {
    pqxx::connection conn("dbname=data user=postgres password=password hostaddr=127.0.0.1 port=5432");

    if (conn.is_open()) {
        std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
    }
    else {
        std::cout << "Can't open database" << std::endl;
        return 1;
    }

    CREATE_TABLE_CouriersData(conn);
    CREATE_TABLE_OrdersData(conn);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/couriers").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res) {
        getCouriers(res, req, conn);
        res.end();
        });

    CROW_ROUTE(app, "/couriers").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req, crow::response& res) {
        createCouriers(res, req, conn);
        res.end();
        });

    CROW_ROUTE(app, "/couriers/<int>").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res, int courier_id) {
        getCourierId(res, req, conn, courier_id);
        res.end();
        });

    CROW_ROUTE(app, "/orders").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req, crow::response& res) {
        createOrders(res, req, conn);
        res.end();
        });

    CROW_ROUTE(app, "/orders/<int>").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res, int order_id) {
        GetOrdersId(res, req, conn, order_id);
        res.end();
        });

    CROW_ROUTE(app, "/orders").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res) {
        GetOrders(res, req, conn);
        res.end();
        });

    CROW_ROUTE(app, "/orders/complete").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req, crow::response& res) {
        completeOrder(res, req, conn);
        res.end();
        });

    app.port(8080).multithreaded().run();
    return 0;
}

