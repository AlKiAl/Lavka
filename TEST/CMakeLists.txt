cmake_minimum_required(VERSION 3.12)
project(Test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Установка пути к корневой директории установки vcpkg
set(VCPKG_ROOT "C:/dev/vcpkg/vcpkg" CACHE STRING "")

# Настройка интеграции vcpkg
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Поиск библиотек gtest, pqxx, и nlohmann/json
find_package(libpqxx REQUIRED)
find_package(gtest REQUIRED)
find_package(nlohmann_json REQUIRED)

# Добавление исходных файлов к проекту
set(SOURCES
    src/Other.cpp
    src/Other.h
    src/TEST_GET.cpp 
    src/TEST_IDEMPOTENCE.cpp 
    src/MAIN_TEST_POST.cpp
)

# Добавление исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES})

# Включение директорий для заголовочных файлов
target_include_directories(${PROJECT_NAME} PRIVATE ${PQXX_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS})

# Линковка с библиотеками
target_link_libraries(${PROJECT_NAME} PRIVATE ${PQXX_LIBRARIES} ${GTEST_LIBRARIES} nlohmann_json::nlohmann_json)


# cmake ..
# cmake --build . --config Release --target Test