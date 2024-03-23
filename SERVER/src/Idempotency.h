#pragma once
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <pqxx/pqxx>

std::string sha256(const std::string& input);
bool checkIdempotencyKeyExists(pqxx::connection& conn, const std::string& idempotencyKey, const std::string& Nametable);
