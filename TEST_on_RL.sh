#!/bin/bash

# Проверяем наличие зависимостей
if ! command -v ab &> /dev/null; then
    echo "Error: Apache Benchmark (ab) is not installed. Please install it and try again."
    exit 1
fi

if ! command -v nc &> /dev/null; then
    echo "Error: Netcat (nc) is not installed. Please install it and try again."
    exit 1
fi

# Получаем каталог, в котором находится скрипт
SCRIPT_DIR=$(dirname "$0")

# Сохраняем текущий рабочий каталог
CURRENT_DIR=$(pwd)

cd "$SCRIPT_DIR/SERVER/src" || { echo "Error: Could not change directory to SERVER/src"; exit 1; }

# освобождаем порт
fuser -k -n tcp 8080

# Запускаем сервер
./main &

# Ждем, пока порт 8080 не станет доступным
while ! nc -z localhost 8080; do   
  sleep 0.1 # Ждем 0.1 секунды перед следующей попыткой
done

# Переходим обратно в исходный каталог
cd "$CURRENT_DIR" || { echo "Error: Could not change directory back to $CURRENT_DIR"; exit 1; }

# Назначаем права доступа
chmod +r post_data_couriers.json post_data_orders.json post_data_orders_complete.json || { echo "Error: Could not change permissions for JSON files"; exit 1; }

# Переменные для URL и количества запросов
URL="http://localhost:8080"
REQUESTS=1000


ab -n "$REQUESTS" -c 10 -p post_data_couriers.json -T "application/json" "$URL/couriers" > couriers_ab_output.txt || echo "Error: POST request to /couriers failed"
ab -n "$REQUESTS" -c 10 -p post_data_orders.json -T "application/json" "$URL/orders" > orders_ab_output.txt || echo "Error: POST request to /orders failed"
ab -n "$REQUESTS" -c 10 -p post_data_orders_complete.json -T "application/json" "$URL/orders/complete" > orders_complete_ab_output.txt || echo "Error: POST request to /orders/complete failed"


ab -n "$REQUESTS" -c 10 "$URL/couriers/1" > couriers_get_ab_output.txt || echo "Error: GET request to /couriers/1 failed"
ab -n "$REQUESTS" -c 10 "$URL/orders/1" > orders_get_ab_output.txt || echo "Error: GET request to /orders/1 failed"
ab -n "$REQUESTS" -c 10 "$URL/couriers?limit=13&offset=1" > couriers_query_ab_output.txt || echo "Error: GET request to /couriers?limit=13&offset=1 failed"
ab -n "$REQUESTS" -c 10 "$URL/orders?limit=13&offset=1" > orders_query_ab_output.txt || echo "Error: GET request to /orders?limit=13&offset=1 failed"


echo "Let's prove the fulfillment of the rate limiter, namely 10 RPS: the formula has the form (Complete requests - Non-2xx response) / [Time taken for tests], where [] is rounding up to an integer. You will get no more than 10, and with several requests you will almost certainly get 10. Non-2xx response -- number of responses with code 429"

