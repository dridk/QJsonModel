cmake_minimum_required(VERSION 3.26)

message(WARNING "MODULE_PATH: ${CMAKE_MODULE_PATH}")

# Additional paths to search for custom and third-party CMake modules
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

include(BuildProperties)

prevent_in_source_build()
disable_deprecated_features()

# When this package is included as a subproject, there's no need to
# build and run the unit-tests.
disable_tests_if_subproject()
#git_setup_submodules()

project(Qt6JsonModel
	VERSION 0.0.2
	LANGUAGES C CXX
	# Save this for later:
	# HOMEPAGE_URL <URL>
	DESCRIPTION "QJsonModel is a json tree model class for Qt6/C++17 based on QAbstractItemModel. MIT License."
)

detect_linkers()

include(CPM)

SET(QJsonModel_TOP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# enable compile_commands.json generation for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS On)

IF(NOT CMAKE_BUILD_TYPE)
	SET( CMAKE_BUILD_TYPE Debug )
ENDIF()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# Disable GNU compiler extensions
oset(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Qt6 COMPONENTS
	Core REQUIRED
	Widgets REQUIRED
	Gui REQUIRED
)
qt_standard_project_setup()

# Set output directories for build targets
#set_artifact_dir(${CMAKE_BINARY_DIR}/out)

include(CheckIncludeFile)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(-DDEBUG=1 -DQDEBUG=1)
endif()

qt_add_library(QJsonModel
OBJECT
	QJsonModel.cpp
)
#set_target_properties(QJsonModel PROPERTIES EXPORT_NAME QJsonModel)
add_library(Qt6::QJsonModel ALIAS QJsonModel)

# Since headers are in the include/ directory, going to handle MOC invocation
# manually.
set_target_properties(QJsonModel PROPERTIES
	AUTOMOC OFF
	POSITION_INDEPENDENT_CODE ON
)

set(QJsonModel_PUBLIC_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
#file(MAKE_DIRECTORY ${Qt6JsonModel_TOP_SOURCE_DIR})

add_custom_target(QJsonModel_include_files COMMAND ${CMAKE_COMMAND} -E copy_directory ${QJsonModel_TOP_SOURCE_DIR}/include ${QJsonModel_PUBLIC_INCLUDE_DIR}
    DEPENDS ${QJsonModel_TOP_SOURCE_DIR}/include
    COMMENT "Copying include files to ${QJsonModel_PUBLIC_INCLUDE_DIR}"
)

target_include_directories(QJsonModel
SYSTEM BEFORE
PUBLIC ${QJsonModel_PUBLIC_INCLUDE_DIR}
)
add_dependencies(QJsonModel QJsonModel_include_files)

# Manually call moc on all header files
file(GLOB_RECURSE QJsonModel_HEADER_FILES
	${QJsonModel_TOP_SOURCE_DIR}/include/**.hpp)

qt_wrap_cpp(QJsonModel_MOC_SOURCES  ${QJsonModel_HEADER_FILES}
	TARGET QJsonModel
)

# Append the MOC files to the source list
target_sources(QJsonModel PUBLIC ${QJsonModel_MOC_SOURCES})

add_library(QJsonModelStatic STATIC)
add_library(Qt6::QJsonModelStatic ALIAS QJsonModelStatic )
set_target_properties(QJsonModelStatic PROPERTIES OUTPUT_NAME "QJsonModel")

add_library(QJsonModelShared SHARED )
add_library(Qt6::QJsonModelShared ALIAS QJsonModelShared )
set_target_properties(QJsonModelShared PROPERTIES OUTPUT_NAME "QJsonModel")

target_link_libraries(QJsonModelStatic PRIVATE QJsonModel)
target_link_libraries(QJsonModelShared PRIVATE QJsonModel)

target_link_libraries(QJsonModel
PUBLIC
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)

# vim: ts=2 sw=2 noet foldmethod=indent :
