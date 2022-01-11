// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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
            const std::vector<std::unique_ptr<SunspecDevice>>& get_devices() const;
            const everest::modbus::ModbusClient& get_modbus_client() const;
            const SunspecDevice& get_device_by_index(int index) const;
            const json get_devices_information() const;
            void print_summary();

        private:
            SunspecDeviceMapping(const SunspecDeviceMapping&) = delete;
            SunspecDeviceMapping& operator=(const SunspecDeviceMapping&) = delete;
            int get_start_address();
            int detect_base_register();
            void add_device(std::unique_ptr<SunspecDevice>& device);
            int start;
            const uint8_t unit_id;
            std::tuple<uint16_t, uint16_t> read_model_header(int model_start_address);
            std::vector<std::unique_ptr<SunspecDevice>> devices;
            const everest::modbus::ModbusClient& modbus_client;

    };

} // namespace sunspec
 } // namespace everest

#endif
