#include "order_handler.h"
#include "Idempotency.h"
#include <nlohmann/json.hpp>
#include <iostream>

#include <string>
#include <iomanip>
#include<time.h> // gmtime_s

using namespace std;
using json = nlohmann::json;


void CREATE_TABLE_OrdersData(pqxx::connection& conn) {

    // completed_time ��������� �� �����, ����� ������ ����� ��������� �����.
    // complete_time == ����� ����������
    std::string query = R"(CREATE TABLE IF NOT EXISTS OrdersData (

                            order_id SERIAL PRIMARY KEY,    
                            order_json JSON,
                            idempotency_key TEXT NOT NULL,
                            complete_time TEXT DEFAULT 'not start order',
                            completed_time TEXT DEFAULT 'not completed order'
                                                                 );
                          )";

    pqxx::work txn(conn);
    pqxx::result result = txn.exec(query);
    txn.commit();

}


std::string GetCompletedTime()
{
    // ��������� �������� �������
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // �������������� ������� � ISO 8601
    std::tm tm;
    gmtime_s(&tm, &now_time_t);
    char buffer[30];
    strftime(buffer, 30, "%Y-%m-%dT%H:%M:%S", &tm);
    return (std::string(buffer) + '.' + std::to_string(milliseconds.count()) + "Z");


}


void createOrders(crow::response& res, const crow::request& req, pqxx::connection& conn)
{

    std::cout << "Handling POST /orders" << std::endl;

    try {

        json JsonRequest = json::parse(req.body);

        if (JsonRequest.find("orders") != JsonRequest.end() && JsonRequest["orders"].is_array()) {

            for (auto& order : JsonRequest["orders"]) {


                if (order["weight"].is_null() || order["regions"].is_null() || order["delivery_hours"].is_null() || order["cost"].is_null()) {
                    throw std::invalid_argument("invalid key");
                }

                const std::string idempotencyKey = sha256(order.dump()); // ��������� ��� �� ������ ���������� ������������� JSON
                if (checkIdempotencyKeyExists(conn, idempotencyKey, "OrdersData")) {

                    pqxx::work txnIdempotency(conn);
                    std::string FindId = "SELECT order_id FROM OrdersData WHERE idempotency_key = $1;";
                    pqxx::result result = txnIdempotency.exec_params(FindId, idempotencyKey);
                    order["order_id"] = result[0][0].as<int>();
                    cout << "order-json already exists :\n" << order.dump(2) << '\n';
                    txnIdempotency.commit();
                    continue;
                }

                const std::string CompletedTime = GetCompletedTime();
                std::string insertQuery = "INSERT INTO OrdersData (order_json, idempotency_key, completed_time) VALUES ($1::json, $2, $3)";


                pqxx::work txn(conn);
                order["completed_time"] = CompletedTime; // ������ ����������� ����� �������� sha256, ��� ��� ��� ������� json ISO 8601 ��������� --> ������������� ���������� sha256 ���� ��� ���������� order.dump()
                txn.exec_params(insertQuery, order.dump(), idempotencyKey, CompletedTime);
                txn.commit();


                pqxx::work txn_id(conn);
                pqxx::result result = txn_id.exec("SELECT order_id FROM OrdersData ORDER BY order_id DESC LIMIT 1;");

                order["order_id"] = result[0][0].as<int>();

                txn_id.commit();
            }


            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(JsonRequest["orders"].dump());


        }
        else {

            throw std::runtime_error("Invalid JSON format");

        }


    }
    catch (const std::exception& e) {
        json DataError = {
            {"error", "bad request"},
            {"message", e.what()}
        };

        cerr << DataError.dump(1) << '\n';

        res.code = 400;


        res.set_header("Content-Type", "application/json");
        res.write("{}");
    }

}


void GetOrdersId(crow::response& res, const crow::request& req, pqxx::connection& conn, const int64_t order_id) {

    std::cout << "Handling GET /orders/id" << std::endl;


    // ��������� ������� ��������� order_id � �������
    if (order_id <= 0) {
        // ���������� ������ 400 Bad Request
        res.code = 400;
        res.set_header("Content-Type", "application/json");
        json DataError = { {"error", "bad request"}, {"message", "Order id is required"} };
        cerr << DataError.dump(1) << endl;
        res.write("{}");
        return;
    }


    // ������������ SQL-������� ��� ��������� ���������� � �������
    std::string query = "SELECT * FROM OrdersData WHERE order_id = $1";

    try {
        // ���������� ������������������ ������� � ���� ������
        pqxx::work txn(conn);
        pqxx::result result = txn.exec_params(query, order_id);

        if (!result.empty()) {

            // �������� �������� JSON-����
            //std::string order_json_str = result[0]["order_json"].as<std::string>();

            // ����������� �������� JSON-���� � JSON-������
            json order = json::parse(result[0]["order_json"].as<std::string>());
            order["order_id"] = order_id;

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(order.dump(2));

        }
        else {
            // ������ � ��������� ��������������� �� ������
            res.code = 404;
            res.set_header("Content-Type", "application/json");
            json DataError = { {"error", "not found"}, {"message", "Order not found"} };
            cerr << DataError.dump(1) << endl;

            res.write("{}");
        }
    }
    catch (const std::exception& e) {
        
        json DataError = { {"error", "internal server error"}, {"message", e.what()} };
        cerr << DataError.dump(1) << endl;

        res.code = 400;
        res.set_header("Content-Type", "application/json");
        
        res.write("{}");
    }
}


void GetOrders(crow::response& res, const crow::request& req, pqxx::connection& conn) {

    cout << "Handling GET /orders" << endl;

    try {
        int64_t offset = req.url_params.get("offset") ? std::stoll(req.url_params.get("offset")) : 0;
        int64_t limit = req.url_params.get("limit") ? std::stoll(req.url_params.get("limit")) : 1;

        if (offset < 0 || limit < 0) {
            throw invalid_argument("incorrect offset or limit");
        }

        json orders = json::array();


        for (int i = offset; i < offset + limit; ++i) {
            crow::response temp_res;
            GetOrdersId(temp_res, req, conn, i);

            if (temp_res.code == 200) {
                json orders_json = json::parse(temp_res.body);
                orders.push_back(orders_json);
            }

        }

        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.write(orders.dump(2));
        cout << orders.dump(2) << '\n';
    }
    catch (const std::exception& e) {
        json DataError = {
            {"error", "bad request"},
            {"message", e.what()}
        };

        cerr << DataError.dump(1) << endl;

        res.code = 400;
        res.set_header("Content-Type", "application/json");
        res.write("{}");
    }
}


void completeOrder(crow::response& res, const crow::request& req, pqxx::connection& conn) {

    std::cout << "Handling POST /orders/complete" << std::endl;

    try {

        // ������� ���� �������
        json jsonRequest = json::parse(req.body);
        //cout << "jsonRequest = \n" << jsonRequest.dump(2) << '\n';

        // ��������, ��� ������ �������� ������ ��������
        if (!jsonRequest["complete_info"].is_array()) {
            res.code = 400;
            cerr << "Expected array of objects" << endl;
            res.write("{}");
            return;
        }

        // ������� ������� ������� � �������
        for (const auto& obj : jsonRequest["complete_info"]) {

            if (obj["courier_id"].is_null() || obj["order_id"].is_null() || obj["complete_time"].is_null()) {
                throw std::invalid_argument("invalid key");
            }

            // ���������� ���������� � �������, ������ � ������� ����������
            const int64_t courier_id = obj["courier_id"].get<int64_t>();
            const int64_t order_id = obj["order_id"].get<int64_t>();
            const std::string complete_time = obj["complete_time"].get<std::string>();

            if (courier_id <= 0 || order_id <= 0) { // �����: �� �������� ������
                res.code = 400;
                cout << "Order or courier not exist\n";
                res.write("{}");
                continue;
            }

            // �������� ������������� ������ � ��� �������
            pqxx::work txn(conn);
            pqxx::result result_order = txn.exec_params("SELECT * FROM OrdersData WHERE order_id = $1", order_id); // �����: �� ������

            //cout << "result_order\n" << result_order[0]["order_json"] << '\n';

            if (result_order.empty()) {
                res.code = 400;
                cerr << "Order not found\n";
                res.write("{}");
                continue;
            }

            pqxx::result result_courer = txn.exec_params("SELECT * FROM CouriersData WHERE id = $1", courier_id); // �����: ��� �������� �� ������� �������
            if (result_courer.empty()) {
                res.code = 400;
                cerr << "Courer not found\n";
                res.write("{}");
                continue;
            }

            // �������������� �������� �� pqxx::result � ������
            std::string order_json_str = result_order[0]["order_json"].as<std::string>();

            // ������� ������ � ������ JSON
            json order_json = json::parse(order_json_str);
            order_json["order_id"] = order_id;



            // �������� ����� ��� ����������� 
            txn.exec_params("UPDATE OrdersData SET complete_time = $1 WHERE order_id = $2", complete_time, order_id);
            txn.commit();

            // ������� ������������� ������������ ������
            res.code = 200;

            res.set_header("Content-Type", "application/json");
            res.write(order_json.dump(2));

        }
    }
    catch (const std::exception& e) {
        json DataError = {
            {"error", "bad request"},
            {"message", e.what()}
        };

        cerr << DataError.dump(1) << endl;

        res.code = 400;
        res.set_header("Content-Type", "application/json");
        res.write("{}");
    }
}