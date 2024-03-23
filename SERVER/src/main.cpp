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

    RateController rateControllerCouriersGet(10, 1000);
    RateController rateControllerCouriersPost(10, 1000);
    RateController rateControllerCouriersGetId(10, 1000);



    RateController rateControllerOrdersPost(10, 1000);
    RateController rateControllerOrdersGetId(10, 1000);
    RateController rateControllerOrdersGet(10, 1000);
    RateController rateControllerOrdersCompletePost(10, 1000);


    CROW_ROUTE(app, "/couriers").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res) {
        if (rateControllerCouriersGet.check()) {
            getCouriers(res, req, conn);
        }
        else {
            res.code = 429; // Статус код "Too Many Requests"            
        }
        res.end();
            });


    CROW_ROUTE(app, "/couriers").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req, crow::response& res) {
        if (rateControllerCouriersPost.check()) {
            createCouriers(res, req, conn);
        }
        else {
            res.code = 429; // Статус код "Too Many Requests"            
        } 
        res.end();

            });


    CROW_ROUTE(app, "/couriers/<int>").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res, int courier_id) {
        
        if (rateControllerCouriersGetId.check()) {
            getCourierId(res, req, conn, courier_id);
        }
        else {
            res.code = 429;
        }
        res.end();
            });



    CROW_ROUTE(app, "/orders").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req, crow::response& res) {
        if (rateControllerOrdersPost.check()) {
            createOrders(res, req, conn);
        }
        else {
            res.code = 429;
        }
        res.end();
            });


    CROW_ROUTE(app, "/orders/<int>").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res, int order_id) {
        
        if (rateControllerOrdersGetId.check()) {
        GetOrdersId(res, req, conn, order_id);
            }
        else {
        res.code = 429;
        }
        res.end();
            });


    CROW_ROUTE(app, "/orders").methods(crow::HTTPMethod::GET)
        ([&](const crow::request& req, crow::response& res) {
        if (rateControllerOrdersGet.check()) {
        GetOrders(res, req, conn);
            }
        else {
        res.code = 429;
        }
        res.end();
            });


    CROW_ROUTE(app, "/orders/complete").methods(crow::HTTPMethod::POST)
        ([&](const crow::request& req, crow::response& res) {
        if (rateControllerOrdersCompletePost.check()) {
        completeOrder(res, req, conn);
            }
        else {
        res.code = 429;
        }
        res.end();
            });



    app.port(8080).multithreaded().run();
    return 0;
}


/*
    GET
http://localhost:8080/couriers?limit=13&offset=1

http://localhost:8080/couriers/6
*/