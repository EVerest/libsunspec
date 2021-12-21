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
#include <iostream>
#include <memory>

#include <boost/log/core.hpp>
#include <everest/logging.hpp>
#include <boost/log/expressions.hpp>

#include <sunspec/reader.hpp> 
#include <sunspec/sunspec_device_mapping.hpp>
#include <sunspec/utils.hpp>
#include <sunspec/config.hpp>

namespace logging = boost::log;
namespace sunspec = everest::sunspec;
using PointType = std::string;

int main(int arg, char **argv) {

    std::unique_ptr<sunspec::config::SunspecConfig> sunspec_config = sunspec::config::load_config_from_file("sunspec/config.ini");
    
    // Initializing TCP connection
    everest::connection::TCPConnection conn(sunspec_config->modbus_config->ip_addr, sunspec_config->modbus_config->port);
    if (!conn.is_valid()) return 0;

    // Initializing modbus client with the given TCP connection
    everest::modbus::ModbusTCPClient modbus_client = everest::modbus::ModbusTCPClient(conn);

    // Initializing device mapping and scanning for models
    sunspec::SunspecDeviceMapping sunspec_device_mapping(modbus_client, 1); // (modbus_client, unit_id)
    sunspec_device_mapping.scan();
    sunspec_device_mapping.print_summary();

    // Instantiating a reader class   
    std::string query_string = sunspec_config->query_string;
    std::cout << "query string: " << query_string << std::endl;
    sunspec::SunspecReader<PointType> s = sunspec::SunspecReader<PointType>(query_string, sunspec_device_mapping);
    s.print_query_summary();

    // Getting query device information
    std::cout << "device info: " << s.get_query_device_information();

    // Getting all query devices information
    std::cout << "all devices info: " << s.get_devices_information();

    // Reading value
    auto read_val = s.read();
    std::cout << "Manufacturer: " << read_val << std::endl;
    // std::cout << " - after scale factor: " << sunspec::utils::apply_scale_factor<PointType>(read_val, s.get_scale_factor()) << std::endl;
    return 0;
}