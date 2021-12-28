// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef SUNSPEC_UTILS_HPP
#define SUNSPEC_UTILS_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>

#include <nlohmann/json.hpp>

#include <modbus/modbus_client.hpp>
#include <sunspec/commons.hpp>
#include <sunspec/types.hpp>

using json = nlohmann::json;

namespace everest { namespace sunspec { namespace utils {

    std::string bytevector_to_string(const std::vector<uint8_t>& bytearray);
    uint16_t bytevector_to_uint16(const std::vector<uint8_t>& bytevector);
    int16_t bytevector_to_int16(const std::vector<uint8_t>& bytevector);
    bool is_sunspec_identifier(const std::string& str);
    bool is_sunspec_identifier(const std::vector<uint8_t>& bytearray);
    bool is_zero_length_model(uint16_t model_id, uint16_t model_length);
    bool is_common_model(uint16_t model_id);
    template <typename IterableType>
    std::string iterable_to_string(const IterableType& iterable);
    json json_from_model_id(const uint16_t model_id);
    bool json_contains(const json& json_, const std::string& key);
    types::ModbusReadFunction make_modbus_read_function(const everest::modbus::ModbusClient& modbus_client, const uint8_t& unit_id);

    template <typename T>
    float apply_scale_factor(T point_value, int16_t scale_factor) {
      return point_value * pow(10, scale_factor);
    }

} // namespace utils
 } // namespace sunspec
  } // namespace everest

#endif
