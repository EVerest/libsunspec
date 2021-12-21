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
#ifndef SUNSPEC_DEVICE_MAPPING_H
#define SUNSPEC_DEVICE_MAPPING_H

#include <tuple>
#include <cstdint>
#include <list>

#include <modbus/modbus_client.hpp>
#include <sunspec/sunspec_device.hpp>

namespace everest { namespace sunspec {

    class SunspecDeviceMapping {
        public:
            SunspecDeviceMapping(everest::modbus::ModbusClient& modbus_client, uint8_t unit_id = 1);
            void scan();
            const std::vector<std::unique_ptr<SunspecDevice>>& get_devices();
            everest::modbus::ModbusClient& get_modbus_client();
            SunspecDevice& get_device_by_index(int index);
            std::vector<json> get_devices_information();
            void print_summary();

        private:
            int get_start_address();
            int detect_base_register();
            void add_device(std::unique_ptr<SunspecDevice>& device);
            int start;
            const uint8_t unit_id;
            std::tuple<uint16_t, uint16_t> read_model_header(int model_start_address);
            std::vector<std::unique_ptr<SunspecDevice>> devices;
            everest::modbus::ModbusClient& modbus_client;

    };

} // namespace sunspec
 } // namespace everest

#endif
