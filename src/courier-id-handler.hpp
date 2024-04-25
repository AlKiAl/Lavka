#pragma once

#include <userver/components/component_list.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <optional>

namespace pg_service_template {

void AppendCourierId(userver::components::ComponentList& component_list);

} // namespace pg_service_template
