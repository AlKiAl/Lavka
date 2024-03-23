#include <gtest/gtest.h>
#include "Other.h"


TEST(GetCheckCodeResponse, CouriersId) {    

    std::string buffer;
    // It is necessary to ensure the availability of json with id 1
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

    const std::string endpoint = "http://localhost:8080/couriers";
    const int code_response = POST(endpoint, data.dump(), buffer);
    ASSERT_NE(code_response, -1);
    EXPECT_EQ(code_response, 200);
    buffer.clear();


    const std::string url_200 = "http://localhost:8080/couriers/2"; // 200
    const std::string url_404 = "http://localhost:8080/couriers/22"; // 404
    const std::string url_400 = "http://localhost:8080/couriers/-2"; // 400
    
    EXPECT_EQ(GET(url_200, buffer), 200);
    EXPECT_NE(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(url_400, buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(url_404, buffer), 404);
    EXPECT_EQ(buffer, "{}");
    
}


TEST(GetCheckCodeResponse, OrdersId) {

    std::string buffer;
    // It is necessary to ensure the availability of json with id 1
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

    const std::string endpoint = "http://localhost:8080/orders";
    const int code_response = POST(endpoint, data.dump(),  buffer);
    ASSERT_NE(code_response, -1);
    EXPECT_EQ(code_response, 200);
    buffer.clear();

    

    const std::string url_200 = "http://localhost:8080/orders/2"; // 200
    const std::string url_404 = "http://localhost:8080/orders/22"; // 404
    const std::string url_400 = "http://localhost:8080/orders/-2"; // 400

    EXPECT_EQ(GET(url_200, buffer), 200);
    EXPECT_NE(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(url_400, buffer), 400);
    cout << "buffer " << buffer << endl;
    EXPECT_EQ(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(url_404, buffer), 404);
    EXPECT_EQ(buffer, "{}");

}



class URL_Param : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализация фикстуры перед каждым тестом
        offset_ = 1;
        limit_ = 13;
    }

    void TearDown() override {
        // Очистка фикстуры после каждого теста
    }

    int offset_;
    int limit_;
};

 
string CouriersMakeURL(const int offset_, const int limit_) {
    return "http://localhost:8080/couriers?limit=" + to_string(offset_) + "&offset=" + to_string(limit_);
}


TEST_F(URL_Param, Couriers) {

    std::string buffer;

    // It is necessary to ensure the availability of json with id 1
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

    const std::string endpoint = "http://localhost:8080/couriers";
    const int code_response = POST(endpoint, data.dump(), buffer);
    ASSERT_NE(code_response, -1);
    EXPECT_EQ(code_response, 200);
    buffer.clear();
    

    const std::string url_400_stoll_err_1 = "http://localhost:8080/couriers?limit=13&offset";

    EXPECT_EQ(GET(url_400_stoll_err_1, buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const std::string url_400_stoll_err_2 = "http://localhost:8080/couriers?limit=13&offset=";

    EXPECT_EQ(GET(url_400_stoll_err_2, buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    // для http://localhost:8080/couriers?limit=13&offset=1 написать фикстуру


    EXPECT_EQ(GET(CouriersMakeURL(offset_, limit_), buffer), 200);
    EXPECT_NE(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(CouriersMakeURL(-offset_, -limit_), buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(CouriersMakeURL(offset_, -limit_), buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(CouriersMakeURL(-offset_, limit_), buffer), 400);
    EXPECT_EQ(buffer, "{}");


}


string OrdersMakeURL(const int offset_, const int limit_) {
    return "http://localhost:8080/orders?limit=" + to_string(offset_) + "&offset=" + to_string(limit_);
}


TEST_F(URL_Param, Orders) {

    std::string buffer;
    // It is necessary to ensure the availability of json with id 1
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

    const std::string endpoint = "http://localhost:8080/orders";
    const int code_response = POST(endpoint, data.dump(), buffer);
    ASSERT_NE(code_response, -1);
    EXPECT_EQ(code_response, 200);
    buffer.clear();


    const std::string url_400_stoll_err_1 = "http://localhost:8080/orders?limit=13&offset";

    EXPECT_EQ(GET(url_400_stoll_err_1, buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const std::string url_400_stoll_err_2 = "http://localhost:8080/orders?limit=13&offset=";

    EXPECT_EQ(GET(url_400_stoll_err_2, buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    EXPECT_EQ(GET(OrdersMakeURL(offset_, limit_), buffer), 200);
    EXPECT_NE(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(OrdersMakeURL(-offset_, -limit_), buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(OrdersMakeURL(offset_, -limit_), buffer), 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();

    EXPECT_EQ(GET(OrdersMakeURL(-offset_, limit_), buffer), 400);
    EXPECT_EQ(buffer, "{}");

}