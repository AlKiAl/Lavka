#pragma once

#include <string>
#include <userver/formats/json.hpp>
#include <userver/components/component_list.hpp>

namespace order_controller {

	void AppendOrderComplete(userver::components::ComponentList& component_list);
	
} // namespace order_controller
