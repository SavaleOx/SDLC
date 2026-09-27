cmake_minimum_required(VERSION 3.16)
project(MyQtApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== ВАЖНО: включаем автоматику Qt =====
set(CMAKE_AUTOMOC ON)   # генерирует moc-код для классов с Q_OBJECT
set(CMAKE_AUTOUIC ON)   # обрабатывает .ui файлы (если будут)
set(CMAKE_AUTORCC ON)   # обрабатывает .qrc файлы (ресурсы)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ===== Собираем ВСЕ .cpp-файлы в проекте =====
file(GLOB SOURCES "*.cpp" "src/*.cpp")

qt_add_executable(MyQtApp ${SOURCES})

target_link_libraries(MyQtApp PRIVATE Qt6::Widgets)