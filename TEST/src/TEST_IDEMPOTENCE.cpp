#include <gtest/gtest.h>
#include "Other.h"


pqxx::connection conn("dbname=data user=postgres password=password hostaddr=127.0.0.1 port=5432");


TEST(Idempotence, Couriers) {
    
    std::string buffer;
    DELETE_TABLE_BY_NAME(conn, "CouriersData");

    const json data = R"(
    {
        "couriers": [
            {
                "courier_type": "FOOT",
                "regions": [1, 2, 3],
                "working_hours": ["08:00-12:00", "14:00-18:00"]
            },
            {
                "courier_type": "BIKE",
                "regions": [4, 5, 6],
                "working_hours": ["09:00-13:00", "15:00-19:00"]
            }
        ]
    }
    )"_json;

     
    for (int i = 0; i < 20; ++i) {
        //cout << '\n' << data["couriers"][0]["regions"][0] << '\n';
        
        const std::string endpoint = "http://localhost:8080/couriers";
        const int code_response = POST(endpoint, data.dump(), buffer);
        ASSERT_NE(code_response, -1);
        EXPECT_EQ(code_response, 200);

        // Создание объекта nontransaction
        pqxx::nontransaction txn(conn);

        // Выполнение запроса на подсчет количества строк
        pqxx::result result_count_line = txn.exec("SELECT COUNT(*) FROM CouriersData;");

        // Извлечение результата запроса
        //int rowCount = result[0][0].as<int>();

        EXPECT_EQ(result_count_line[0][0].as<int>(), data["couriers"].size());

    }

}


TEST(Idempotence, Orders){

    std::string buffer;
    DELETE_TABLE_BY_NAME(conn, "OrdersData");

    const json data = R"(
    {
      "orders": [
        {
          "weight": 2,
          "regions": 1,
          "delivery_hours": ["10:00-12:00", "15:00-18:00"],
          "cost": 500
        },
        {
          "weight": 1,
          "regions": 4,
          "delivery_hours": ["09:00-11:00", "14:00-16:00"],
          "cost": 300
        }
      ]
    }
    )"_json;


    for (int i = 0; i < 20; ++i) {
        
        const std::string endpoint = "http://localhost:8080/orders";
        const int code_response = POST(endpoint, data.dump(), buffer);
        ASSERT_NE(code_response, -1);
        EXPECT_EQ(code_response, 200);

        // Создание объекта nontransaction
        pqxx::nontransaction txn(conn);

        // Выполнение запроса на подсчет количества строк
        pqxx::result result_count_line = txn.exec("SELECT COUNT(*) FROM OrdersData;");

        // Извлечение результата запроса
        //int rowCount = result[0][0].as<int>();
        
        EXPECT_EQ(result_count_line[0][0].as<int>(), data["orders"].size());


    }

}


TEST(Idempotence, OrdersComplete) {

    std::string buffer;

    const json data = R"(
{
  "complete_info": [
    {
      "courier_id": 1,
      "order_id": 1,
      "complete_time": "2024-02-21T11:37:43.422Z"
    }
  ]
}
        )"_json;


    // Создание объекта nontransaction
    pqxx::nontransaction txn(conn);

    // Выполнение запроса на подсчет количества строк
    const pqxx::result result_count_line = txn.exec("SELECT COUNT(*) FROM OrdersData;");


    for (int i = 0; i < 20; ++i) {

        const std::string endpoint = "http://localhost:8080/orders/complete";

        const int code_response = POST(endpoint, data.dump(), buffer);
        ASSERT_NE(code_response, -1);
        EXPECT_EQ(code_response, 200);
        

        const pqxx::result result_current_count_line = txn.exec("SELECT COUNT(*) FROM OrdersData;");
        EXPECT_EQ(result_count_line[0][0].as<int>(), result_current_count_line[0][0].as<int>());

        
        pqxx::result check_complete_time = txn.exec("SELECT complete_time FROM OrdersData WHERE order_id = 1;");

        EXPECT_NE(check_complete_time[0][0].as<string>(), "not start order");

    }

}