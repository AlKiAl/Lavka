#pragma once

#include <userver/components/component_list.hpp>
#include <string>

namespace order_controller {

	//std::string OrderId::HandleRequestThrow(const userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) ;

	void AppendOrderId(userver::components::ComponentList& component_list);
  
}
