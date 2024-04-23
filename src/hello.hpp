#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace pg_service_template {

	enum class UserType { kFirstTime, kKnown }; // it can be uodate in HandleRequestThrow --> SayHelloTo
	std::string SayHelloTo(std::string_view name, UserType type);

	void AppendHello(userver::components::ComponentList& component_list);

}  // namespace pg_service_template
