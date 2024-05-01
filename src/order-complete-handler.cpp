#include "order-complete-handler.hpp"
#include "orders-handler.hpp"
#include "order-id-handler.hpp"

#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/formats/serialize/common_containers.hpp>

#include <userver/components/component.hpp>

#include <iostream>
#include <string>
using namespace std;

namespace order_controller {

	  using namespace userver::formats::json; // for: FromString; ValueBuilder	
	  using namespace userver;
	  

	class OrderComplete final : public userver::server::handlers::HttpHandlerBase {
	 
	 
	 public:
	 
	 
	  static constexpr std::string_view kName = "handler-order-complete";
	  userver::storages::postgres::ClusterPtr pg_cluster_;

		  OrderComplete(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& component_context) 
			  : HttpHandlerBase(config, component_context), 
			  pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {
			  
			    //https://userver.tech/d0/d89/pg_types.html
			    
			    constexpr auto kCreateTable = R"(		    
								create table if not exists complete_info (
								    courier_id int,
								    order_id int,
								    complete_time text,

								    foreign key (order_id) references OrdersData(id),
								    foreign key (courier_id) references CouriersData(id),
								    primary key (order_id)
								);
						  )";		  

			  using storages::postgres::ClusterHostType;
			  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable); 
			  			  
			  }


		  std::string HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override {

			    const std::string& body = request.RequestBody();
			    formats::json::Value Array_complete_info = userver::formats::json::FromString(body);

			if(!Array_complete_info.HasMember("complete_info") || !Array_complete_info["complete_info"].IsArray()){
				
				request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
				
				cout << "error in complete_info\n";
				return "{}";	
				
			}
			
			
			ValueBuilder builder;	  	
		  	
			for(const formats::json::Value& info : Array_complete_info["complete_info"]){
			
				
				if(!info.HasMember("courier_id") || !info.HasMember("order_id") ||!info.HasMember("complete_time")){
					
					request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
					cout << "not key\n";
					return "{}";	
					
				}
					
				try{
				  auto result = pg_cluster_->Execute(/*пробелы в конце строк запроса необходимы!*/
				      userver::storages::postgres::ClusterHostType::kMaster,
				      "INSERT INTO complete_info(courier_id, order_id, complete_time) VALUES($1, $2, $3) ON CONFLICT DO NOTHING "
				      ,
				      info["courier_id"].As<int>(), info["order_id"].As<int>(), info["complete_time"].As<string>());
				      
				  }catch(...){
					  request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
					  cout << "not exist\n";
				  	return "{}";	
				  }
				      												
			
				    auto order = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
			         		"select order_json, completed_time from public.OrdersData where id=$1;", info["order_id"].As<int>());

	    
	    
				    ValueBuilder buffer = ValueBuilder(order[0][0].As<Value>()); // сли что: см. o-h.cpp line 186
				    buffer["order_id"] = info["order_id"].As<int>();
				    buffer["completed_time"] = order[0][1].As<string>();
				    builder.PushBack(move(buffer));
				    	
			
			}	
						  	
		  	
    			return ToString(builder.ExtractValue());
		  }
	  
	  
	};


	void AppendOrderComplete(userver::components::ComponentList& component_list) {
	  component_list.Append<OrderComplete>();
	}


} // namespace order_controller
