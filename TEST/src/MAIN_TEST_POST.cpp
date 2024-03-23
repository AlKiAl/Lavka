#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include "Other.h"


TEST(PostCheckCodeResponse, Couriers) {

    std::string buffer;
    const std::string endpoint = "http://localhost:8080/couriers";
    int code_response = -1;


    const json data = R"(
    {
        "couriers": [
            {
                "courier_type": "BIKE",
                "regions": [91, 82, 13],
                "working_hours": ["18:00-22:00", "23:00-23:01"]
            }
        ]
    }
    )"_json;

    code_response = POST(endpoint, data.dump(), buffer);
    EXPECT_EQ(code_response, 200);
    EXPECT_NE(buffer, "{}");
    buffer.clear();


    const json data_error_couriers = R"(
    {
        "Couriers": [
            {
                "courier_type": "FOOT",
                "regions": [1, 2, 3],
                "working_hours": ["08:00-12:00", "14:00-18:00"]
            }
        ]
    }
    )"_json;
    
    code_response = POST(endpoint, data_error_couriers.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_error_courier_type = R"(
    {
        "couriers": [
            {
                "Courier_type": "FOOT",
                "regions": [1, 2, 3],
                "working_hours": ["08:00-12:00", "14:00-18:00"]
            }
        ]
    }
    )"_json;

    code_response = POST(endpoint, data_error_courier_type.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_error_regions = R"(
    {
        "couriers": [
            {
                "courier_type": "BIKE",
                "Regions": [91, 82, 13],
                "working_hours": ["18:00-22:00", "23:00-23:01"]
            }
        ]
    }
    )"_json;

    code_response = POST(endpoint, data_error_regions.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_error_working_hours = R"(
    {
        "couriers": [
            {
                "courier_type": "BIKE",
                "regions": [91, 82, 13],
                "Working_hours": ["18:00-22:00", "23:00-23:01"]
            }
        ]
    }
    )"_json;

    code_response = POST(endpoint, data_error_working_hours.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");

}


TEST(PostCheckCodeResponse, Orders) {

    std::string buffer;
    const std::string endpoint = "http://localhost:8080/orders";
    int code_response = -1;

    const json data = R"(
    {
      "orders": [
        {
          "weight": 2,
          "regions": 1,
          "delivery_hours": ["10:00-12:00", "15:00-18:00"],
          "cost": 500
        }
      ]
    }
    )"_json;

    
    code_response = POST(endpoint, data.dump(), buffer);
    EXPECT_EQ(code_response, 200);
    EXPECT_NE(buffer, "{}");
    buffer.clear();


    const json data_error_orders = R"(
    {
      "Orders": [
        {
          "weight": 2,
          "regions": 1,
          "delivery_hours": ["10:00-12:00", "15:00-18:00"],
          "cost": 500
        }
      ]
    }
    )"_json;


    code_response = POST(endpoint, data_error_orders.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_error_weight = R"(
    {
      "orders": [
        {
          "Weight": 2,
          "regions": 1,
          "delivery_hours": ["10:00-12:00", "15:00-18:00"],
          "cost": 500
        }
      ]
    }
    )"_json;


    code_response = POST(endpoint, data_error_weight.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_error_regions = R"(
    {
      "orders": [
        {
          "weight": 2,
          "Regions": 1,
          "delivery_hours": ["10:00-12:00", "15:00-18:00"],
          "cost": 500
        }
      ]
    }
    )"_json;


    code_response = POST(endpoint, data_error_regions.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();



    const json data_error_delivery_hours = R"(
    {
      "orders": [
        {
          "weight": 2,
          "regions": 1,
          "Delivery_hours": ["10:00-12:00", "15:00-18:00"],
          "cost": 500
        }
      ]
    }
    )"_json;


    code_response = POST(endpoint, data_error_delivery_hours.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();



    const json data_error_cost = R"(
    {
      "orders": [
        {
          "weight": 2,
          "regions": 1,
          "delivery_hours": ["10:00-12:00", "15:00-18:00"],
          "Cost": 500
        }
      ]
    }
    )"_json;


    code_response = POST(endpoint, data_error_cost.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
}


TEST(PostCheckCodeResponse, OrdersComplete) {

    std::string buffer;
    const std::string endpoint = "http://localhost:8080/orders/complete";
    int code_response = -1;

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


    
    code_response = POST(endpoint, data.dump(), buffer);
    EXPECT_EQ(code_response, 200);
    EXPECT_NE(buffer, "{}");
    buffer.clear();
    


    const json data_error_complete_info = R"(
{
  "Complete_info": [
    {
      "courier_id": 1,
      "order_id": 1,
      "complete_time": "2024-02-21T11:37:43.422Z"
    }
  ]
}
        )"_json;



    code_response = POST(endpoint, data_error_complete_info.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_error_courier_id = R"(
{
  "complete_info": [
    {
      "Courier_id": 1,
      "order_id": 1,
      "complete_time": "2024-02-21T11:37:43.422Z"
    }
  ]
}
        )"_json;



    code_response = POST(endpoint, data_error_courier_id.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_order_id = R"(
{
  "complete_info": [
    {
      "courier_id": 1,
      "Order_id": 1,
      "complete_time": "2024-02-21T11:37:43.422Z"
    }
  ]
}
        )"_json;



    code_response = POST(endpoint, data_order_id.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
    buffer.clear();


    const json data_error_complete_time = R"(
{
  "complete_info": [
    {
      "courier_id": 1,
      "order_id": 1,
      "Complete_time": "2024-02-21T11:37:43.422Z"
    }
  ]
}
        )"_json;



    code_response = POST(endpoint, data_error_complete_time.dump(), buffer);
    EXPECT_EQ(code_response, 400);
    EXPECT_EQ(buffer, "{}");
}


int main() {
    
    // Инициализация Google Test
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();

}
