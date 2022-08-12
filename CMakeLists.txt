cmake_minimum_required(VERSION 3.14)

project(sunspec
    VERSION 0.1
)

find_package(everest-cmake 0.1 REQUIRED
    PATHS ../everest-cmake
)

# options
option(BUILD_EXAMPLES "Build examples" OFF)
option(SUNSPEC_INSTALL "Install the library (shared data might be installed anyway)" ${EVC_MAIN_PROJECT})

# dependencies
find_package(Boost COMPONENTS program_options REQUIRED)

if (NOT DISABLE_EDM)
    evc_setup_edm()

    # In EDM mode, we can't install exports (because the dependencies usually do not install their exports)
    set(SUNSPEC_INSTALL OFF)
else()
    find_package(nlohmann_json REQUIRED)
    find_package(everest-log REQUIRED)
    find_package(everest-modbus REQUIRED)
endif()

add_library(sunspec)
add_library(everest::sunspec ALIAS sunspec)
target_sources(sunspec
    PRIVATE
        src/sunspec_device_mapping.cpp
        src/sunspec_device.cpp src/utils.cpp
        src/sunspec_model.cpp
        src/conversion.cpp
)

target_link_libraries(sunspec
    PUBLIC
        everest::modbus
        nlohmann_json::nlohmann_json
    PRIVATE
        everest::log
)

target_include_directories(sunspec
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_subdirectory(src)

# packaging
install(
    DIRECTORY aux/sunspec_models/json/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/sunspec/models
)

if (SUNSPEC_INSTALL)
    install(
        TARGETS sunspec
        EXPORT sunspec-exports
    )

    install(
        DIRECTORY include/ lib/
        TYPE INCLUDE
    )

    install(
        TARGETS sunspec_device_cli_scanner
    )

    install(
        FILES config.ini
        DESTINATION ${CMAKE_INSTALL_DATADIR}/everest/sunspec/
    )

    evc_setup_package(
        NAME everest-sunspec
        NAMESPACE everest
        EXPORT sunspec-exports
        ADDITIONAL_CONTENT
            "find_dependency(nlohmann_json)"
            "find_dependency(everest-log)"
    )
endif()


if (BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()
