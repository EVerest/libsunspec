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
#ifndef SUNSPEC_CONFIG_H
#define SUNSPEC_CONFIG_H

#include <string>
#include <sstream>
#include <memory>
#include <stdexcept>

#include <everest/logging.hpp>
#include <inih/INIReader.h>

#include <sunspec/exceptions.hpp>

namespace everest { namespace sunspec { namespace config {

    struct ModbusTCPConfig {
        std::string ip_addr;
        int port;
        int unit_id;
    };

    struct SunspecConfig {
        std::unique_ptr<ModbusTCPConfig> modbus_config;
        std::string query_string;
    };

    void check_required_parameter(const std::string& param_name, const std::string& str) {
        if (str == "UNKNOWN") {
            std::stringstream error_msg;
            error_msg << "No config value given for parameter '" << param_name << "'.";
            EVLOG(error) << error_msg.str();
            throw exceptions::input_error(error_msg.str());
        }
    }

    std::unique_ptr<SunspecConfig> load_config_from_file(const std::string& filepath) {

        // Loading ini config file
        INIReader reader(filepath);
        int errcode = reader.ParseError();
        if (errcode != 0) {
            std::stringstream error_msg;
            error_msg << "Could not load filepath with filepath " << filepath << " - error code: " << errcode;
            EVLOG(error) << error_msg.str();
            throw exceptions::input_error(error_msg.str());
        }

        // Building config struct
        std::unique_ptr<ModbusTCPConfig> modbus_config(new ModbusTCPConfig);
        std::unique_ptr<SunspecConfig> sunspec_config_(new SunspecConfig);

        modbus_config->ip_addr = reader.Get("MODBUS", "IP", "UNKNOWN");
        check_required_parameter("IP", modbus_config->ip_addr);
        modbus_config->port = reader.GetInteger("MODBUS", "Port", 502);
        modbus_config->unit_id = reader.GetInteger("MODBUS", "UnitID", 1);

        sunspec_config_->query_string = reader.Get("Sunspec", "PointQuery", "UNKNOWN");
        check_required_parameter("PointQuery", sunspec_config_->query_string);
        sunspec_config_->modbus_config = std::move(modbus_config);

        return sunspec_config_;
    };

} // namespace config
 } // namespace sunspec
  } // namespace everest

#endif