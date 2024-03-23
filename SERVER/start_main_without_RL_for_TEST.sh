#!/bin/bash

cd ./src

# освобождаем порт
fuser -k -n tcp 8080

# Запускаем сервер
./main_without_RL_for_TEST &

# Ожидаем завершения сервера
wait $!

# После завершения сервера освобождаем порт
fuser -k -n tcp 8080

