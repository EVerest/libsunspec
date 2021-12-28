// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>

#include <everest/logging.hpp>
#include <nlohmann/json.hpp>

#include <sunspec/utils.hpp>
#include <sunspec/exceptions.hpp>

#include <connection/utils.hpp>

using namespace everest::sunspec;
using json = nlohmann::json;

bool utils::json_contains(const json& json_, const std::string& key) {
    return json_.find(key) != json_.end();
}

bool utils::is_sunspec_identifier(const std::string& str) {
    bool res = str == commons::SUNS_IDENTIFIER;
    EVLOG(debug) << "Checking if string is the SunS identifier: " << str << ". Result: " << res;
    return res;
}

bool utils::is_sunspec_identifier(const std::vector<uint8_t>& bytevector) {
    EVLOG(debug) << "Checking if bytevector is the SunS identifier: " << everest::connection::utils::get_bytes_hex_string(bytevector);
    return bytevector[0] == commons::SUNS_IDENTIFIER[0] &&
           bytevector[1] == commons::SUNS_IDENTIFIER[1] &&
           bytevector[2] == commons::SUNS_IDENTIFIER[2] &&
           bytevector[3] == commons::SUNS_IDENTIFIER[3];
}

bool utils::is_zero_length_model(uint16_t model_id, uint16_t model_length) {
    return model_id == commons::ZERO_LENGTH_MODEL_ID && model_length == commons::ZERO_LENGTH_MODEL_LENGTH;
}

bool utils::is_common_model(uint16_t model_id) {
    return model_id == commons::COMMON_MODEL_ID;
}

types::ModbusReadFunction utils::make_modbus_read_function(const everest::modbus::ModbusClient& modbus_client, const uint8_t& unit_id) {
    return [&] (uint16_t first_register_address, uint16_t num_registers_to_read) {
        return modbus_client.read_holding_register(unit_id, first_register_address, num_registers_to_read);
    };
}

template <typename IterableType>
std::string utils::iterable_to_string(const IterableType& iterable)  {
    std::stringstream outbuffer;
    for (const auto& n : iterable) {
        outbuffer << n << " ";
    }
    outbuffer << std::endl;
    return outbuffer.str();
}

json utils::json_from_model_id(const uint16_t model_id) {
    // Reading model id file
    std::stringstream filename;
    filename << commons::JSON_MODELS_BASE_FOLDER << "/model_" << model_id << ".json";
    EVLOG(debug) << "SunspecModel::SunspecModel() - Reading file " << filename.str();
    std::ifstream model_file(filename.str());
    json j;
    model_file >> j;
    return j;
}
