cmake_minimum_required(VERSION 3.26)
project(Qt6JsonModel
	VERSION 0.0.2
	LANGUAGES C CXX
	# Save this for later:
	DESCRIPTION "QJsonModel is a json tree model class for Qt6/C++17 based on QAbstractItemModel. MIT License."
)

IF(NOT CMAKE_BUILD_TYPE)
        SET( CMAKE_BUILD_TYPE Debug )
ENDIF()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(QJsonModel_PUBLIC_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)

find_package(Qt6 COMPONENTS
	Core REQUIRED
	Widgets REQUIRED
	Gui REQUIRED
)
qt_standard_project_setup()
qt_add_library(QJsonModel OBJECT
	QJsonModel.cpp
        include/QJsonModel.hpp
)
add_library(Qt6::QJsonModel ALIAS QJsonModel)
target_include_directories(QJsonModel PUBLIC
        ${QJsonModel_PUBLIC_INCLUDE_DIR}
)
target_link_libraries(QJsonModel PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

add_library(QJsonModelStatic STATIC)
add_library(Qt6::QJsonModelStatic ALIAS QJsonModelStatic )
set_target_properties(QJsonModelStatic PROPERTIES OUTPUT_NAME "QJsonModel")
target_link_libraries(QJsonModelStatic PUBLIC QJsonModel)
target_include_directories(QJsonModelStatic PUBLIC ${QJsonModel_PUBLIC_INCLUDE_DIR})

add_library(QJsonModelShared SHARED)
add_library(Qt6::QJsonModelShared ALIAS QJsonModelShared)
set_target_properties(QJsonModelShared PROPERTIES OUTPUT_NAME "QJsonModel")
target_link_libraries(QJsonModelShared PUBLIC QJsonModel)
target_include_directories(QJsonModelShared PUBLIC ${QJsonModel_PUBLIC_INCLUDE_DIR})

# vim: ts=2 sw=2 noet foldmethod=indent :
