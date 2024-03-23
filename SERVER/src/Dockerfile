# Источник Dockerfile: https://www.youtube.com/watch?v=B9_yPXAlLiM&ab_channel=Tocardinhio

# Используем образ GCC
FROM gcc:latest

# Устанавливаем необходимые зависимости
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    libboost-dev \
    libboost-regex-dev \
    libasio-dev \
    libpqxx-dev \
    libpq-dev \
    nlohmann-json3-dev \
    libcrypto++-dev

# Устанавливаем рабочую директорию
WORKDIR /app

# Клонируем репозиторий Crow
RUN git clone https://github.com/CrowCpp/Crow.git

# Переходим в директорию Crow
WORKDIR /app/Crow

# Создаем директорию для сборки и собираем Crow
RUN mkdir build && cd build && cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF && make install

# Переходим обратно в основную директорию приложения
WORKDIR /app

# Копируем исходный код в контейнер
COPY . .

# Компилируем приложение
RUN g++ -o main main.cpp courier_handler.cpp order_handler.cpp Idempotency.cpp RateLimit.cpp -lpthread  -lpqxx -lpq -lcryptopp

# Запускаем приложение при старте контейнера
CMD ["./main"]
