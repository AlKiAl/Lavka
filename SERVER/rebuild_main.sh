#!/bin/bash

cd ./src

# Собираем программу
g++ -o main main.cpp courier_handler.cpp order_handler.cpp Idempotency.cpp RateLimit.cpp -lpthread -lpqxx -lpq -lcryptopp


