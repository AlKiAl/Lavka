#main_without_RL_for_TEST and main in folder SERVER

Используют библиотеку: 

cryptopp
pqxx 
crow 
nlohmann/json

#test_executable in folder TEST

Использует библиотеку: 

gtest
pqxx
nlohmann/json


#About lib

Рекомендуется использовать [conan](https://conan.io/center) или [vcpkg](https://vcpkg.io/en/packages), они существенно упрощают управление зависимостями и все указанные библиотеки есть там. 

В противном случае придется работать через командую строку:

		sudo apt-get update && sudo apt-get install -y \
		    git \
		    cmake \
		    libboost-dev \
		    libboost-regex-dev \
		    libasio-dev \
		    libpqxx-dev \
		    libpq-dev \
		    nlohmann-json3-dev \
		    libcrypto++-dev
    
а gtest и crow придется собирать вручную: 

##gtest

[gtest](https://gist.github.com/Cartexius/4c437c084d6e388288201aadf9c8cdd5)

		sudo apt-get install libgtest-dev

		sudo apt-get install cmake # install cmake

		cd /usr/src/gtest

		sudo cmake CMakeLists.txt

		sudo make

		sudo cp *.a /usr/lib

		sudo ln -s /usr/lib/libgtest.a /usr/local/lib/gtest/libgtest.a

		sudo ln -s /usr/lib/libgtest_main.a /usr/local/lib/gtest/libgtest_main.a

##crow

		# Устанавливаем рабочую директорию
		touch app
		cd ./app
		
		# Клонируем репозиторий Crow
		git clone https://github.com/CrowCpp/Crow.git

		# Переходим в директорию Crow
		/app/Crow

		# Создаем директорию для сборки и собираем Crow
		RUN mkdir build && cd build && cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF && make install

# Start script

Перед запуском скриптов из их родной дирректории выполните: 

		find . -maxdepth 1 -type f -name "*.sh" -exec chmod +x {} \; #Эта команда ищет все файлы в текущей директории с расширением .sh и добавляет им права на выполнение (+x).


##TEST.sh

Скрипт TEST.sh запускает тест на соответствие Code Responses и Body Responses из openapi.json. В этом скрипте используются файлы из папки TEST. 
В нем используется файл main_without_RL_for_TEST из SERVER, в котором отсут-т проверка на Rate limiter -- это необходимо чтобы не получать ответ 429 вместо 200 или 400. 



##TEST_on_RL.sh 

Скрипт TEST_on_RL.sh запускает тест на Rate limiter. В этом скрипте используется утилита ab. 

В нем для POST-запроса используются файлы  

post_data_couriers.json 
post_data_orders.json
post_data_orders_complete.json

с выводом в  

couriers_ab_output.txt
orders_ab_output.txt
orders_complete_ab_output.txt
couriers_get_ab_output.txt
orders_get_ab_output.txt
couriers_query_ab_output.txt
orders_query_ab_output.txt


В нем используется файл main из SERVER, в котором присут-т проверка на Rate limiter

