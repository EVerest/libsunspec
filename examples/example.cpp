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
#include <cstdint>
#include <vector>

#include <sunspec/sunspec_device_mapping.hpp>
#include <sunspec/utils.hpp>

int main() {
   
    everest::connection::TCPConnection conn("192.168.2.134", 502);
    if (!conn.is_valid()) return 0;
    everest::modbus::ModbusTCPClient modbus_client = everest::modbus::ModbusTCPClient(conn);
    everest::sunspec::SunspecDeviceMapping sunspec_device_mapping(modbus_client, 1);
    sunspec_device_mapping.scan();

    for (const auto& device : sunspec_device_mapping.get_devices()) {
        std::cout << "device starting at " << device->get_start_address() << std::endl;
        device->print_name_index_map();
        std::cout << device->get_device_information() << std::endl;
        //for (const auto& model : device->get_models()) {
        //    std::cout << "id: " << model->model_id << " offset: " << model->offset << " model name: " << model->get_name() << std::endl;
        //    for (const auto& point : model->get_points()) {
        //        auto& p = point.second;
        //        std::cout << p->get_name() << " - label: " << p->get_point_def()["label"] << std::endl;
        //        if (p->requires_scale_factor()) {
        //            std::cout << "sf: " << p->get_scale_factor() << std::endl;
        //        }
        //    }
        //}
    }

    return 0;
}
