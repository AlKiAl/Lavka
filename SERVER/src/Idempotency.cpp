#include "Idempotency.h"
#include <iostream>

bool checkIdempotencyKeyExists(pqxx::connection& conn, const std::string& idempotencyKey, const std::string& Nametable) {
    try {
        // ��������� SQL-������ ��� �������� ������������� ������ � ������ idempotencyKey
        pqxx::work txn(conn);
        //std::string query = "SELECT 1 FROM OrdersData WHERE idempotency_key = " + txn.quote(idempotencyKey);
        std::string query = "SELECT 1 FROM " + Nametable + " WHERE idempotency_key = $1";
        pqxx::result result = txn.exec_params(query, idempotencyKey);

        // ���� ��������� ������� �� ������, �� ������ � ������ idempotencyKey  ����������
        return !result.empty();
    }
    catch (const std::exception& e) {
        // � ������ ������ ������� ��������� �� ������ � ���������� false
        std::cerr << "Error while checking idempotency key: " << e.what() << std::endl;
        return false;
    }
}


std::string sha256(const std::string& input) {
    CryptoPP::SHA256 hash;
    std::string digest;
    CryptoPP::StringSource(input, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(digest)
            )
        )
    );
    return digest;
}