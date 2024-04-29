#include "orders-handler.hpp"

#include <userver/formats/json.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/formats/serialize/common_containers.hpp>

#include <userver/crypto/hash.hpp>


#include <fmt/format.h>

#include <userver/components/component.hpp> // 
/*#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>*/


#include <userver/utils/assert.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <userver/storages/postgres/result_set.hpp>


#include <iomanip>
#include<time.h> // gmtime_s
#include <iostream>
#include <string>
using namespace std;

namespace order_controller {


	string GetCompletedTime(){

	    // Получение текущего времени
	    auto now = std::chrono::system_clock::now();
	    auto now_time_t = std::chrono::system_clock::to_time_t(now);
	    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	    // Получение времени в структуре tm
	    std::tm tm;
	    gmtime_r(&now_time_t, &tm); // Первый аргумент - указатель на time_t, второй аргумент - указатель на tm

	    // Форматирование времени в строку в соответствии с ISO 8601
	    char buffer[30];
	    strftime(buffer, 30, "%Y-%m-%dT%H:%M:%S", &tm);
	    return (std::string(buffer) + '.' + std::to_string(milliseconds.count()) + "Z");
	    
	}


	  using namespace userver::formats::json; // for: FromString; ValueBuilder	
	  using namespace userver;
	  
	class Orders final : public userver::server::handlers::HttpHandlerBase {

	 public:
	 
		  static constexpr std::string_view kName = "handler-orders";
		  userver::storages::postgres::ClusterPtr pg_cluster_;


		  Orders(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& component_context)
		  : HttpHandlerBase(config, component_context),
		    pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {	
			
			    //https://userver.tech/d0/d89/pg_types.html
			    // std::string -- not type?
			    constexpr auto kCreateTable = R"~(
			    			    CREATE TABLE IF NOT EXISTS OrdersData (

						    id SERIAL PRIMARY KEY,    
						    order_json JSON,
						    json_hash CHAR(64) UNIQUE,
						    completed_time TEXT 
						                   );
						  )~";
						  
			  using storages::postgres::ClusterHostType;
			  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable); // Execute() [3/4]			
			
			}



		  std::string HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override {

		    switch (request.GetMethod()) {
		      case userver::server::http::HttpMethod::kPost:
			return postOrders(request);
		      case userver::server::http::HttpMethod::kGet:
			return getOrders(request);
		    }

		  }


	 private:
	 
	 
		  std::string postOrders(const userver::server::http::HttpRequest& request) const {
		  
			    const std::string& body = request.RequestBody();
			    formats::json::Value Array_orders = userver::formats::json::FromString(body);

			if(!Array_orders.HasMember("orders") || !Array_orders["orders"].IsArray()){
				
				request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
				return "{}";	
				
			}
			
			
			ValueBuilder builder;
			for(const formats::json::Value& order : Array_orders["orders"]){
			
				
				if(!order.HasMember("weight") || !order.HasMember("regions") ||!order.HasMember("delivery_hours") ||!order.HasMember("cost")){
					
					request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
					return "{}";	
					
				}
					
					
				  const string JsonHash = crypto::hash::Sha256(ToString(order));
				  const string TimeISO = GetCompletedTime();
				  
				  auto result = pg_cluster_->Execute(/*пробелы в конце строк запроса необходимы!*/
				      userver::storages::postgres::ClusterHostType::kMaster,
				      "INSERT INTO OrdersData(order_json, json_hash, completed_time) VALUES($1, $2, $3) "
				      "ON CONFLICT (json_hash) DO NOTHING "
				      "RETURNING OrdersData.id"
				      ,
				      formats::json::Value(order), JsonHash, TimeISO);
				      								

				  if (result.IsEmpty()) {
				    
				    result = pg_cluster_->Execute(
				      userver::storages::postgres::ClusterHostType::kMaster,
				      "SELECT id FROM OrdersData WHERE json_hash =  $1"
				      ,
				      JsonHash);	
				  }
				  
				  ValueBuilder buffer = ValueBuilder(order);
				  buffer["order_id"] = result.AsSingleRow<int>();
				  buffer["completed_time"] = TimeISO;
				  
				  builder.PushBack(buffer.ExtractValue());
				  								
				
			}
	    
	    
	    		return ToString(builder.ExtractValue());
	    		
		  }



		  std::string getOrders(const userver::server::http::HttpRequest& request) const {
		  
			    int limit = 1, offset = 0;
			    if (request.HasArg("limit")){limit = stoi(request.GetArg("limit"));}
			      
			    if (request.HasArg("offset")){offset = stoi(request.GetArg("offset"));}
			    

			    auto orders_res = pg_cluster_->Execute(
				userver::storages::postgres::ClusterHostType::kMaster,
				"select id, order_json, completed_time  from OrdersData offset $1 limit $2;",
				offset,
				limit
				);
			
			    userver::formats::json::ValueBuilder result_json;

			    for(storages::postgres::Row row : orders_res){

			    	//ValueBuilder Buffer = ValueBuilder(FromString(row["order_json"].As<string>()));
			    	ValueBuilder Buffer = ValueBuilder(row["order_json"].As<Value>());
			    				    	
			    	//ValueBuilder Buffer = row["order_json"].As<ValueBuilder>(); 
			    	Buffer["order_id"] = row["id"].As<int>(); 
			    	Buffer["completed_time"] = row["completed_time"].As<string>(); 
			    	
			    	result_json.PushBack(Buffer.ExtractValue());
			    
			    }				    
			    
			    if(result_json.IsEmpty()){
			    	return "{}";
			    }
			    
			    return ToString(result_json.ExtractValue()) ;
		  }
		  

		};


		void AppendOrders(userver::components::ComponentList& component_list) {
		  component_list.Append<Orders>();
		}


}
