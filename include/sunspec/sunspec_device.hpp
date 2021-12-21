/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SUNSPEC_DEVICE_H
#define SUNSPEC_DEVICE_H

#include <memory>
#include <tuple>
#include <list>
#include <cstdint>

#include <sunspec/sunspec_model.hpp>
#include <modbus/modbus_client.hpp>

namespace everest { namespace sunspec {

    class SunspecDeviceMapping;

    class SunspecDevice {
        public:
            SunspecDevice(SunspecDeviceMapping& device_mapping, const int mapped_start, const uint8_t unit_id = 1);
            int get_start_address();
            void add_model(const uint16_t model_id, uint16_t model_length, const int offset);
            const std::vector<std::unique_ptr<SunspecModel>>& get_models();
            const SunspecModel& get_model_by_name(const std::string& name) const;
            everest::modbus::ModbusClient& get_modbus_client();
            const uint8_t& get_unit_id();
            void print_name_index_map() const;
            json get_device_information();

        private:
            std::tuple<uint16_t, uint16_t> read_model_header(int model_start_address);
            uint8_t unit_id;
            int mapped_start;
            std::vector<std::unique_ptr<SunspecModel>> models;
            std::map<std::string, int> model_name_to_index;         // discarding the possibility of having one model with the same name for the same device
            SunspecDeviceMapping& device_mapping;
    };
} // namespace sunspec
 } // namespace everest

#endif