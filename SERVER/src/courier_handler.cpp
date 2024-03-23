#include "courier_handler.h"
#include "RateLimit.h"
#include "Idempotency.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;


//void getCouriers(crow::response& res, const crow::request& req, pqxx::connection& conn) {
//    cout << "Handling GET /couriers" << endl;
//    int offset = req.url_params.get("offset") ? std::stoi(req.url_params.get("offset")) : 0;
//    int limit = req.url_params.get("limit") ? std::stoi(req.url_params.get("limit")) : 1;
//
//    std::string query = "SELECT * FROM CouriersData OFFSET $1 LIMIT $2";
//    pqxx::work txn(conn);
//    pqxx::result rows = txn.exec_params(query, offset, limit);
//
//    if (!rows.empty()) {
//        json couriers;
//        couriers["couriers"] = json::array();
//
//        for (const auto& row : rows) {
//            json courier;
//            courier["courier_id"] = row["id"].as<int>();
//            courier["courier_type"] = row["courier_type"].as<string>();
//            
//
//            std::stringstream regions_stream(row["regions"].c_str());
//            json regions = json::array();
//            for (string region_value; regions_stream >> region_value;) { // если вместо string буюет int -- это ошибка!!!
//                regions.push_back(region_value); // move?
//            }
//
//            std::stringstream working_hours_stream(row["working_hours"].c_str());
//            json working_hours = json::array();
//            for (std::string time_value; working_hours_stream >> time_value;) {
//                working_hours.push_back(time_value);
//            }
//
//
//
//            courier["regions"] = regions;
//            courier["working_hours"] = working_hours;
//
//            couriers["couriers"].push_back(courier);
//        }
//
//        couriers["limit"] = limit;
//        couriers["offset"] = offset;
//
//        res.code = 200;
//        res.set_header("Content-Type", "application/json");
//        res.write(couriers.dump(2));
//        cout << couriers.dump(2) << '\n';
//    }
//    else {
//        res.code = 404;
//        res.set_header("Content-Type", "application/json");
//        json error_json = { {"error", "not found"}, {"message", "No couriers found"} };
//        res.write(error_json.dump());
//    }
//}


void getCourierId(crow::response& res, const crow::request& req, pqxx::connection& conn, const int64_t courier_id) {


    // Проверяем наличие параметра order_id в запросе
    if (courier_id <= 0) {
        // Возвращаем ошибку 400 Bad Request
        res.code = 400;
        res.set_header("Content-Type", "application/json");
        json error_json = { {"error", "bad request"}, {"message", "courier id is required"} };
        cout << error_json.dump(1) << '\n';
        res.write("{}");
        return;
    }


    std::cout << "Handling GET /couriers/id" << std::endl;
    // Формирование SQL-запроса для получения информации о курьере
    std::string query = "SELECT * FROM CouriersData WHERE id = $1";


    try {
        // Выполнение параметризованного запроса к базе данных
        pqxx::work txn(conn);
        pqxx::result result = txn.exec_params(query, courier_id);

        if (!result.empty()) {
            // Формирование JSON-ответа
            json courier;
            //courier["courier_id"] = result[0]["id"].as<int>();
            courier["courier_id"] = courier_id;
            courier["courier_type"] = result[0]["courier_type"].as<string>();


            std::stringstream regions_stream(result[0]["regions"].c_str());
            json regions = json::array();
            for (std::string region_value; regions_stream >> region_value;) {
                regions.push_back(region_value);
            }


            std::stringstream working_hours_stream(result[0]["working_hours"].c_str());
            json working_hours = json::array();
            for (std::string time_value; working_hours_stream >> time_value;) {
                working_hours.push_back(time_value);
            }

            courier["regions"] = regions;
            courier["working_hours"] = working_hours;

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(courier.dump());
        }
        else {

            // Курьер с указанным идентификатором не найден
            res.code = 404;
            res.set_header("Content-Type", "application/json");
            json DataError = { {"error", "not found"}, {"message", "Courier not found"} };
            cerr << DataError.dump(1) << '\n';
            res.write("{}");
        }
    }
    catch (const std::exception& e) {
        json DataError = {
            {"error", "bad request"},
            {"message", e.what()}
        };

        res.code = 400;
        cerr << DataError.dump(1) << endl;

        res.set_header("Content-Type", "application/json");
        res.write("{}");
    }


}


void getCouriers(crow::response& res, const crow::request& req, pqxx::connection& conn) {

    cout << "Handling GET /couriers" << endl ;


    try {


        int64_t offset = req.url_params.get("offset") ? std::stoll(req.url_params.get("offset")) : 0;
        int64_t limit = req.url_params.get("limit") ? std::stoll(req.url_params.get("limit")) : 1;

        if (offset < 0 || limit < 0) {
            throw invalid_argument("incorrect offset or limit");
        }

        json couriers;
        couriers["couriers"] = json::array();

        for (int i = offset; i < offset + limit; ++i) {
            crow::response temp_res;
            getCourierId(temp_res, req, conn, i);
            if (temp_res.code == 200) {
                json courier_json = json::parse(temp_res.body);
                couriers["couriers"].push_back(courier_json);
            }

        }

        couriers["limit"] = limit;
        couriers["offset"] = offset;

        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.write(couriers.dump(1));
        
    }
    catch (const std::exception& e) {


        json DataError = {
            {"error", "bad request"},
            {"message", e.what()}
        };

        res.code = 400;

        cerr << DataError.dump(1) << endl;
        res.set_header("Content-Type", "application/json");
        res.write("{}");
    }
}




void createCouriers(crow::response& res, const crow::request& req, pqxx::connection& conn) {

    std::cout << "Handling POST /couriers" << std::endl;

    try {
        auto jsonRequest = json::parse(req.body);

        if (jsonRequest.find("couriers") != jsonRequest.end() && jsonRequest["couriers"].is_array()) {


            for (auto& courier : jsonRequest["couriers"]) {



                if (!courier.contains("courier_type") || !courier.contains("regions") || !courier.contains("working_hours")) {
                    throw std::invalid_argument("invalid key");
                }


                const std::string idempotencyKey = sha256(courier.dump()); // Вычисляем хеш на основе строкового представления JSON
                if (checkIdempotencyKeyExists(conn, idempotencyKey, "CouriersData")) {

                    // в courier вставить id 
                    pqxx::work txn_id(conn);
                    pqxx::result result = txn_id.exec("SELECT id FROM CouriersData WHERE idempotency_key = '" + idempotencyKey + "';");
                    txn_id.commit(); // Завершаем транзакцию перед созданием новой

                    courier["courier_id"] = result[0][0].as<int>();

                    cout << "courier-json already exists : \n" << courier.dump(2) << '\n';
                    continue;
                }

                // в courier вставить id 
                pqxx::work txn_id(conn);
                pqxx::result result = txn_id.exec("SELECT id FROM CouriersData ORDER BY id DESC LIMIT 1;");
                txn_id.commit(); // Завершаем транзакцию перед созданием новой

                if (!result.empty()) {
                    courier["courier_id"] = result[0][0].as<int>() + 1;
                }
                else {
                    // Обработка случая, когда таблица пуста
                    // Например, присваиваем начальное значение courier_id
                    courier["courier_id"] = 1;
                }


                
                // Convert JSON arrays to PostgreSQL array literals
                std::string regions = "{";
                for (auto& region : courier["regions"]) {
                    regions += std::to_string(region.get<int>()) + ",";
                }
                regions.pop_back(); // Remove the trailing comma
                regions += "}";

                
                                 
                std::string working_hours = "{";
                for (auto& hours : courier["working_hours"]) {
                    working_hours += "\"" + hours.get<std::string>() + "\",";
                }
                working_hours.pop_back(); // Remove the trailing comma
                working_hours += "}";
                //std::cout << working_hours << '\n';
                if (working_hours == "{}") { throw; }


                std::string insertQuery = "INSERT INTO CouriersData (courier_type, regions, working_hours, idempotency_key) VALUES ($1, $2, $3, $4)";
                pqxx::work txn(conn);

                txn.exec_params(insertQuery,
                    courier["courier_type"].get<std::string>(),
                    regions,
                    working_hours,
                    idempotencyKey
                );

                txn.commit();


            }


            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(jsonRequest.dump());
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

        res.code = 400;
        res.set_header("Content-Type", "application/json");
        cerr << DataError.dump(1) <<'\n';
        res.write("{}");
    }
}




void CREATE_TABLE_CouriersData(pqxx::connection& conn) {

    try {

        {
            pqxx::work txn1(conn);
            pqxx::result r1 = txn1.exec("SELECT 1 FROM pg_type WHERE typname = 'courier_type_';");
            if (!r1.empty()) {
                std::cout << "Type 'courier_type' already exists" << std::endl;
            }
            else {
                // Запрос на создание типа данных ENUM
                std::string create_enum_query = "CREATE TYPE courier_type_ AS ENUM ('FOOT', 'BIKE', 'AUTO');";
                txn1.exec(create_enum_query);
                std::cout << "Created ENUM type successfully" << std::endl;
            }
            txn1.commit(); // Фиксация изменений в конце блока
        }

        // error!!!
        //// Проверка существования типа данных ENUM
        //{
        //    pqxx::work txn1(conn);
        //    pqxx::result r1 = txn1.exec("SELECT 1 FROM pg_type WHERE typname = 'CourierType';");
        //    if (!r1.empty()) {
        //        std::cout << "Type 'CourierType' already exists" << std::endl;
        //    }
        //    else {
        //        // Запрос на создание типа данных ENUM
        //        std::string create_enum_query = "CREATE TYPE CourierType AS ENUM ('FOOT', 'BIKE', 'AUTO');";
        //        txn1.exec(create_enum_query);
        //        
        //        std::cout << "Created ENUM type successfully" << std::endl;
        //    }
        //    txn1.commit();
        //}


        {
            std::string query = R"(
CREATE TABLE IF NOT EXISTS CouriersData 
    (
        id SERIAL PRIMARY KEY, 
        courier_type courier_type_, 
        regions INT[], 
        working_hours TEXT[],
        idempotency_key TEXT NOT NULL
    )
)";
            pqxx::work txn(conn);
            pqxx::result result = txn.exec(query);
            txn.commit();
        }

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;

    }

}
