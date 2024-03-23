#!/bin/bash

cd ./src

# Собираем программу
g++ -o main_without_RL_for_TEST main_without_RL_for_TEST.cpp courier_handler.cpp order_handler.cpp Idempotency.cpp RateLimit.cpp -lpthread  -lpqxx -lpq -lcryptopp


