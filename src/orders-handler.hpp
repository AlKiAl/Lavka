#pragma once

#include <userver/components/component_list.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <optional>
#include <string_view>
namespace order_controller {



void AppendOrders(userver::components::ComponentList& component_list);
} // namespace order_controller



