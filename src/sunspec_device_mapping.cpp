// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <everest/logging.hpp>
#include <memory>

#include <modbus/modbus_client.hpp>
#include <sunspec/sunspec_device_mapping.hpp>
#include <sunspec/utils.hpp>
#include <sunspec/sunspec_model.hpp>
#include <sunspec/exceptions.hpp>
#include <sunspec/conversion.hpp>
#include <connection/utils.hpp>

using namespace everest::sunspec;

SunspecDeviceMapping::SunspecDeviceMapping(everest::modbus::ModbusClient& modbus_client, uint8_t unit_id) 
    : modbus_client(modbus_client),
      unit_id(unit_id),
      start(-1) {
          EVLOG(debug) << "Initializing SunspecDeviceMapping with unit_id=" << (int) unit_id;
          this->start = this->detect_base_register();
}

int SunspecDeviceMapping::detect_base_register() {
  std::vector<uint8_t> read_result;
  for (const int& current_reg : commons::BASE_REGISTER_ADDRESSES) {
      read_result = this->modbus_client.read_holding_register(this->unit_id, current_reg, 2);
      EVLOG(debug) << "Received bytes sequence during scan: " << everest::connection::utils::get_bytes_hex_string(read_result);
      if ( utils::is_sunspec_identifier(read_result) ) {
        EVLOG(info) << "Found base register at address " << current_reg << ". Looking for models and device maps...";
        return current_reg;
      }
  }
  std::stringstream error_msg;
  error_msg << "Could not detect candidate for base register for device. Is this really a Sunspec conformant device?";
  EVLOG(error) << error_msg.str();
  throw exceptions::scanning_error(error_msg.str());
}

std::tuple<uint16_t, uint16_t> SunspecDeviceMapping::read_model_header(int model_start_address) {
  std::vector<uint8_t> model_header_bytes = this->modbus_client.read_holding_register(this->unit_id, model_start_address, commons::MODEL_INFO_LEN); 
  std::vector<uint8_t> model_id_bytes(model_header_bytes.begin(), model_header_bytes.begin() + 2);
  std::vector<uint8_t> model_length_bytes(model_header_bytes.begin() + 2, model_header_bytes.begin() + 4);
  uint16_t model_id = conversion::bytevector_to_uint16(model_id_bytes);
  uint16_t model_length = conversion::bytevector_to_uint16(model_length_bytes);
  return std::make_tuple(model_id, model_length);
}

const std::vector<std::unique_ptr<SunspecDevice>>& SunspecDeviceMapping::get_devices() const {
  return this->devices;
}

const everest::modbus::ModbusClient& SunspecDeviceMapping::get_modbus_client() const {
  return this->modbus_client;
}

void SunspecDeviceMapping::add_device(std::unique_ptr<SunspecDevice>& device) {
    if (device) {
      this->devices.push_back( std::move(device) );
      EVLOG(debug) << "Added device with " << this->devices.back()->get_models().size() << " models to mapping.";
    }
}

const SunspecDevice& SunspecDeviceMapping::get_device_by_index(int index) const {
  if (index < 0 || index >= this->devices.size()) {
    std::stringstream error_msg;
    error_msg << "Trying to read device with index " << index << " from device mapping having " << this->devices.size() << " devices. (index >= size)";
    EVLOG(error) << error_msg.str();
    throw exceptions::model_access_error(error_msg.str());
  }
  return *(this->devices.at(index));
}

const json SunspecDeviceMapping::get_devices_information() const {
  json devices_info = json::array();
  for (const auto& device : this->devices) {
    devices_info.push_back( device->get_device_information() );
  }
  return devices_info;
}

void SunspecDeviceMapping::print_summary() {
  for (int i = 0 ; i < this->devices.size() ; i++) {
    const auto& device = this->devices[i];
    std::cout << "Summary for device with index " << i << ":" << std::endl;
    for (const auto& model : device->get_models()) {
      std::cout << "\t model id: " << model->model_id << " - model name: " << model->get_name() << std::endl;
    }
  }
}

void SunspecDeviceMapping::scan() {

    if (this->start == -1) {
        throw exceptions::scanning_error("Trying to find models before finding the base register address!");
    }

    std::tuple<uint16_t, uint16_t> header_info;
    uint16_t model_id = 0x00, model_length = 0x00;
    int start_address = this->start + commons::MODEL_INFO_LEN;
    int model_count = 0;
    std::unique_ptr<SunspecDevice> device; 
    while (true) {

        // Reading model header (id + length)
        EVLOG(debug) << "Reading model header in start address: " << start_address;
        header_info = this->read_model_header(start_address);
        model_id = std::get<0>(header_info);
        model_length = std::get<1>(header_info);
        EVLOG(info) << "Detected model ID: " << model_id << " with model length " << model_length;

        if (utils::is_common_model(model_id)) {
          // Saving device state to devices list and reinitializing
          this->add_device(device);          
          EVLOG(debug) << "Attempting to create SunspecDevice with start address offset=" << start_address << ", unit_id=" << this->unit_id;
          device = std::make_unique<SunspecDevice>((*this), start_address, this->unit_id);
          EVLOG(info) << "Created new SunspecDevice with the address offset " << start_address;
        }

        if (utils::is_zero_length_model(model_id, model_length)) {
          // Adding final device and exiting.
	        this->add_device(device);
          EVLOG(info) << "Found Sunspec Zero-Length model at address: " << start_address << " Device count: " << this->devices.size() << " Model count: " << model_count;
          break;
        }

        device->add_model(model_id, model_length, start_address);
        model_count++;
        start_address += model_length + 2;

    }
}
