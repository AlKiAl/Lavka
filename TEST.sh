# Получаем каталог, в котором находится скрипт
SCRIPT_DIR=$(dirname "$0")

# Сохраняем текущий рабочий каталог
CURRENT_DIR=$(pwd)

cd "$SCRIPT_DIR/SERVER/src" || { echo "Error: Could not change directory to SERVER/src"; exit 1; }

# освобождаем порт
fuser -k -n tcp 8080

# Запускаем сервер
./main_without_RL_for_TEST &

# Ждем, пока порт 8080 не станет доступным
while ! nc -z localhost 8080; do   
  sleep 0.1 # Ждем 0.1 секунды перед следующей попыткой
done

# Переходим обратно в исходный каталог
cd "$CURRENT_DIR" || { echo "Error: Could not change directory back to $CURRENT_DIR"; exit 1; }

cd ./TEST/src

./test_executable
