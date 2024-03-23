#include "Other.h"


// Эта функция будет вызвана libcurl для обработки полученных данных
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    size_t total_size = size * nmemb;
    buffer->append((char*)contents, total_size);
    return total_size;
}


int GET(const std::string& url, std::string& buffer) {
    CURL* curl = curl_easy_init();

    int response_code = -1;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return -1;
        }
        else {
            // Получение кода ответа
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            std::cout << "Response code: " << response_code << std::endl;
        }


        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return -1;
    }

    curl_global_cleanup();

    return response_code;
}


void DELETE_TABLE_BY_NAME(pqxx::connection& conn, const std::string& table_name) {
    try {
        // Begin transaction
        pqxx::work txn(conn);

        // Проверяем существование таблицы по имени
        std::string exists_query = "SELECT to_regclass($1)";
        pqxx::result result = txn.exec_params(exists_query, table_name);

        if (!result.empty()) {
            // Таблица существует, удаляем все строки из нее
            std::string delete_query = "TRUNCATE " + conn.esc(table_name) + " RESTART IDENTITY";
            txn.exec(delete_query);

            // Фиксируем изменения
            txn.commit();

            std::cout << "All rows deleted from table '" << table_name << "' successfully" << std::endl;
        }
        else {
            std::cout << "Table '" << table_name << "' does not exist" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


int POST(const std::string& url, const std::string& jsonData, std::string& buffer) {
    CURL* curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    struct curl_slist* headers = curl_slist_append(NULL, "Content-Type: application/json");
    if (!headers) {
        std::cerr << "Failed to set HTTP headers" << std::endl;
        curl_easy_cleanup(curl);
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());


    int response_code = -1;
    CURLcode res;
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }
    else {
        // Получение кода ответа
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        //std::cout << "Response code: " << response_code << std::endl;
    }


    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return response_code;
}
