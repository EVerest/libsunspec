cmake_minimum_required(VERSION 3.14.7)

# set the project name
project(everest_sunspec VERSION 0.1)

include_directories(${PROJECT_SOURCE_DIR}/lib/)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(EDM REQUIRED)

SET(Boost_USE_STATIC_LIBS ON)
include_directories( ${Boost_INCLUDE_DIR} )

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH "..." FORCE)
endif()

add_library(${PROJECT_NAME} src/sunspec_device_mapping.cpp src/sunspec_device.cpp src/utils.cpp src/sunspec_model.cpp src/conversion.cpp)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${BOOST_VARIANT_LIBRARY}
        everest::log
        everest_modbus
        nlohmann_json::nlohmann_json
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
        ${PROJECT_SOURCE_DIR}/lib/inih/
)

add_executable(sunspec_example examples/example.cpp)
target_link_libraries(sunspec_example ${PROJECT_NAME} everest_modbus nlohmann_json::nlohmann_json)

add_executable(read_manufacturer_example examples/read_manufacturer_example.cpp)
target_link_libraries(read_manufacturer_example ${PROJECT_NAME} everest_modbus nlohmann_json::nlohmann_json everest::log)

add_executable(read_inverter_current_example examples/read_inverter_current_example.cpp)
target_link_libraries(read_inverter_current_example ${PROJECT_NAME} everest_modbus nlohmann_json::nlohmann_json everest::log)
install(TARGETS read_manufacturer_example read_inverter_current_example DESTINATION sunspec/examples)

add_subdirectory(aux)
install(FILES config.ini DESTINATION sunspec/)
