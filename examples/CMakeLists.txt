add_executable(read_manufacturer_example read_manufacturer_example.cpp)
target_link_libraries(read_manufacturer_example
    PRIVATE
        nlohmann_json::nlohmann_json
        everest::log
        sunspec
)

add_executable(read_inverter_current_example read_inverter_current_example.cpp)
target_link_libraries(read_inverter_current_example
    PRIVATE
        nlohmann_json::nlohmann_json
        everest::log
        sunspec
)

file(
    COPY config.ini
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
