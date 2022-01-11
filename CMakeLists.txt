cmake_minimum_required(VERSION 3.14.7)

# set the project name
project(sunspec VERSION 0.1)

include_directories(${PROJECT_SOURCE_DIR}/lib/)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(EDM REQUIRED)

SET(Boost_USE_STATIC_LIBS ON)
include_directories( ${Boost_INCLUDE_DIR} )

option(BUILD_EXAMPLES "Build examples" OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH "..." FORCE)
endif()

add_library(libsunspec src/sunspec_device_mapping.cpp src/sunspec_device.cpp src/utils.cpp src/sunspec_model.cpp src/conversion.cpp)
add_library(everest::sunspec ALIAS libsunspec)

target_link_libraries(libsunspec
    PRIVATE
        ${BOOST_VARIANT_LIBRARY}
        everest::log
        everest::modbus
        nlohmann_json::nlohmann_json
)

target_include_directories(libsunspec
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
        ${PROJECT_SOURCE_DIR}/lib/inih/
)


add_subdirectory(aux)
add_subdirectory(src)
install(FILES config.ini DESTINATION sunspec/)

if (BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()
