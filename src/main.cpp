#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "hello.hpp"

#include "couriers-handler.hpp"
#include "courier-id-handler.hpp"

#include "orders-handler.hpp"
#include "order-id-handler.hpp"
#include "order-complete-handler.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>();


  pg_service_template::AppendHello(component_list);
  
  courier_controller::AppendCouriers(component_list);
  courier_controller::AppendCourierId(component_list);

  order_controller::AppendOrders(component_list);
  order_controller::AppendOrderId(component_list);
  order_controller::AppendOrderComplete(component_list);
  
  return userver::utils::DaemonMain(argc, argv, component_list);
}

/*

http://localhost:8080/couriers

http://localhost:8080/couriers/1

http://localhost:8080/couriers?limit=2&offset=11

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



http://localhost:8080/orders 

http://localhost:8080/orders/2 

http://localhost:8080/orders?limit=13&offset=2 

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



http://localhost:8080/orders/complete

{
  "complete_info": [
    {
      "courier_id": 1,
      "order_id": 1,
      "complete_time": "2024-02-21T11:37:43.422Z"
    },
    {
      "courier_id": 2,
      "order_id": 2,
      "complete_time": "2024-02-21T11:37:43.422Z"
    }    
  ]
}
*/
