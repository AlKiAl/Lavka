#include "couriers-handler.hpp"

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <iostream>
using namespace std;

namespace pg_service_template {



    // for class with component
    namespace {
    
	  using namespace userver::formats::json; // for: FromString; ValueBuilder	
	  using namespace userver;
		  
		  
	    class Couriers final : public userver::server::handlers::HttpHandlerBase {

	    public:


	      userver::storages::postgres::ClusterPtr pg_cluster_;	    
	      static constexpr std::string_view kName = "handler-couriers";


	      
	      	      Couriers(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& component_context)
		  : HttpHandlerBase(config, component_context),
		    pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {		    

		      
			  constexpr auto kCreateTable = R"~(
				    CREATE TABLE IF NOT EXISTS CouriersData 
				    (
					id SERIAL PRIMARY KEY, 
					courier_json VARCHAR

				    );
			    )~";


			  using storages::postgres::ClusterHostType;
			  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable); // Execute() [3/4]		    
		    
		    }
		    

	      std::string HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override {
	      
	      	    switch (request.GetMethod()) {
		      case userver::server::http::HttpMethod::kPost:
			return postCouriers(request);
		      case userver::server::http::HttpMethod::kGet:
			return getCouriers(request);
    		    }
	      
	      }
	      
	      
	      
	       private:
 
 
		  std::string postCouriers(const userver::server::http::HttpRequest& request) const {
		  
		  //formats::json::Value Buffer(FromString(request.RequestBody()));
		  const std::string body = request.RequestBody();
			  		  	
    		    string str = "{}";
			ValueBuilder builder;
			
			//for( formats::json::Value   courier : FromString(body)){ // what type of json does it have? WORKING!!!
			for(const formats::json::Value&  courier : FromString(body)["couriers"]){	// WORKING!!!
			//for( formats::json::Value&   courier : FromString(body)){ // error!
			
	
			  		
			  auto result = pg_cluster_->Execute(
			      userver::storages::postgres::ClusterHostType::kMaster,
			      "INSERT INTO CouriersData(courier_json) VALUES($1) "
			      //"ON CONFLICT (courier_json) DO NOTHING"
			      "RETURNING CouriersData.id"
			      ,
			      ToString(courier));
			      				
			  /*if (result.IsEmpty()) {
			    request.SetResponseStatus(server::http::HttpStatus::kNotFound);
			    return "courier exist";
			  }*/
			  
			  formats::json::ValueBuilder buffer = ValueBuilder(courier);
			  buffer["courier_id"] = result.AsSingleRow<int>();

			  builder["couriers"].PushBack(buffer.ExtractValue());
			  
			  																	
			}    		    
    		    
		  
		    return ToString(builder.ExtractValue());
		  }



		  std::string getCouriers(const userver::server::http::HttpRequest& request) const {
		  
		    int limit = 1, offset = 0;
		    if (request.HasArg("limit")){limit = stoi(request.GetArg("limit"));}
		      
		    if (request.HasArg("offset")){offset = stoi(request.GetArg("offset"));}
		    
		    /*auto couriers_res = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			"select courier_json from CouriersData offset $1 limit $2;",
			offset,
			limit
			);		    

		    userver::formats::json::ValueBuilder result_json;
		    for(auto str : couriers_res){

		    	result_json["couriers"].PushBack(FromString(str.As<string>()));
		    
		    }*/
		    
		    auto couriers_res = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			"select id, courier_json from CouriersData offset $1 limit $2;",
			offset,
			limit
			);		    

		    userver::formats::json::ValueBuilder result_json;
		    for(storages::postgres::Row row : couriers_res){

		    	//result_json["couriers"].PushBack(str.AsSingleRow<Value>("courier_json"));
		    	cout << row["courier_json"].As<string>() << '\n' << row["id"].As<int>() << '\n';
		    	ValueBuilder Buffer = ValueBuilder(FromString(row["courier_json"].As<string>()));
		    	
		    	Buffer["courier_id"] = row["id"].As<int>(); // without this line not error!
		    	result_json["couriers"].PushBack(Buffer.ExtractValue());
		    
		    }		    
		    

		    
		    return ToString(result_json.ExtractValue());
		  }
		  
	      
	 };
	      
    }  // namespace




    void AppendCouriers(userver::components::ComponentList& component_list) {
      component_list.Append<Couriers>(); // Courier defenition in namespace{/**/};
    }

}  // namespace pg_service_template

/*
http://localhost:8080/couriers
http://localhost:8080/couriers?limit=2&offset=0
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
*/

/*
psql -h localhost -p 15433 -U testsuite pg_service_template_db_1
SELECT * FROM CouriersData;
*/
