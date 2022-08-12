add_executable(sunspec_device_cli_scanner device_cli_scanner.cpp)

target_link_libraries(sunspec_device_cli_scanner
    PRIVATE
        Boost::program_options
        sunspec
        everest::log
)
