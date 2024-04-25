#include "courier-id-handler.hpp"

#include <userver/formats/json.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_list.hpp>


#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/result_set.hpp>

#include <iostream>

namespace pg_service_template {


	class CourierId final : public userver::server::handlers::HttpHandlerBase {
	
	 public:
	 
		  static constexpr std::string_view kName = "handler-courier-id";
		  userver::storages::postgres::ClusterPtr pg_cluster_;

      	      CourierId(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& component_context)
		  : HttpHandlerBase(config, component_context),
		    pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}



		  std::string HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override {

		    if (!request.HasPathArg("courier_id")) {
			request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
			return "{}";
		    }

		    int id = stoi(request.GetPathArg("courier_id"));

		    auto courier = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
				         "select * from public.CouriersData where id=$1;", id);

		    
		    if (courier.IsEmpty()) {
			request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
			return "{}";	    	
		    }
		    
		    return "not error";
		  }
		  
	  
	}; // end CourierId


	void AppendCourierId(userver::components::ComponentList& component_list) {
	  component_list.Append<CourierId>();
	}


} // namespace pg_service_template
