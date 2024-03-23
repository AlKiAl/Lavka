#!/bin/bash

# Собираем программу
g++ -std=c++17 -pthread -o test_executable Other.cpp TEST_GET.cpp TEST_IDEMPOTENCE.cpp MAIN_TEST_POST.cpp -lgtest -lgtest_main -lpqxx -lpq -lcurl

# Запускаем 
./test_executable
