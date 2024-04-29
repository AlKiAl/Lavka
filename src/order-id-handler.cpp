#include "order-id-handler.hpp"

#include <userver/formats/json.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <userver/components/component.hpp> 

#include <iostream>

using namespace std;

namespace order_controller {

	  using namespace userver::formats::json; // for: FromString; ValueBuilder	
	  using namespace userver;
	  
	class OrderId final : public userver::server::handlers::HttpHandlerBase {
	
	 public:
	 
	  static constexpr std::string_view kName = "handler-order-id";
	  userver::storages::postgres::ClusterPtr pg_cluster_;

	  OrderId(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& component_context)
	      : HttpHandlerBase(config, component_context),
		pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}



	  std::string HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override {
	
	      const int id = stoi(request.GetPathArg("order_id"));
	      
	      if (!request.HasPathArg("order_id") || id <= 0) {
		request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
		return "{}";
	      }
	      

	    auto order = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
			         "select order_json, completed_time from public.OrdersData where id=$1;", id);

	    
	    if (order.IsEmpty()) {
		request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
		return "{}";	    	
	    }
	    
	    ValueBuilder buffer = ValueBuilder(order[0][0].As<Value>()); // сли что: см. o-h.cpp line 186
	    buffer["order_id"] = id;
	    buffer["completed_time"] = order[0][1].As<string>();
	    
	    return ToString(buffer.ExtractValue());
	      
	    }

	};


	void AppendOrderId(userver::components::ComponentList& component_list) {
	    component_list.Append<OrderId>();
	}

} // namespace order_controller

