#pragma once

//#ifndef ROUTES_H
//#define ROUTES_H

#include <crow.h>
#include <pqxx/pqxx>

void getCourierId(crow::response& res, const crow::request& req, pqxx::connection& conn, int64_t courier_id);
void getCouriers(crow::response& res, const crow::request& req, pqxx::connection& conn);
void createCouriers(crow::response& res, const crow::request& req, pqxx::connection& conn);
void CREATE_TABLE_CouriersData(pqxx::connection& conn);



//#endif // ROUTES_H


