cmake_minimum_required(VERSION 3.26)
project(
  Qt6JsonModel
  VERSION 0.0.2
  LANGUAGES CXX
  DESCRIPTION
    "QJsonModel is a json tree model class for Qt6/C++17 based on QAbstractItemModel. MIT License."
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
qt_standard_project_setup()
qt_add_library(QJsonModel OBJECT QJsonModel.cpp include/QJsonModel.hpp)
add_library(Qt6::QJsonModel ALIAS QJsonModel)
target_link_libraries(QJsonModel PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
target_include_directories(QJsonModel PUBLIC include)

add_library(QJsonModelStatic STATIC)
add_library(Qt6::QJsonModelStatic ALIAS QJsonModelStatic)
if(WIN32)
  set_target_properties(QJsonModelStatic PROPERTIES OUTPUT_NAME "QJsonModelLIB")
else()
  set_target_properties(QJsonModelStatic PROPERTIES OUTPUT_NAME "QJsonModel")
endif()
target_link_libraries(QJsonModelStatic PUBLIC QJsonModel)

add_library(QJsonModelShared SHARED)
add_library(Qt6::QJsonModelShared ALIAS QJsonModelShared)
set_target_properties(QJsonModelShared PROPERTIES OUTPUT_NAME "QJsonModel")
target_link_libraries(QJsonModelShared PUBLIC QJsonModel)

# vim: ts=2 sw=2 noet foldmethod=indent :
