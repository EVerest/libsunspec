add_executable(sunspec_device_cli_scanner device_cli_scanner.cpp)

find_package(Boost COMPONENTS program_options REQUIRED)
target_link_libraries(sunspec_device_cli_scanner
    PRIVATE
        nlohmann_json::nlohmann_json
        everest::sunspec
        everest::modbus
        everest::log
        Boost::program_options
)

# needs c++ 14
target_compile_features(sunspec_device_cli_scanner PRIVATE cxx_std_14)
install(TARGETS sunspec_device_cli_scanner DESTINATION tools/)
