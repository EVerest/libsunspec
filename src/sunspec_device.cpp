// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <sstream>
#include <tuple>

#include <everest/logging/logging.hpp>

#include <sunspec/sunspec_device_mapping.hpp>
#include <sunspec/sunspec_device.hpp>
#include <sunspec/utils.hpp>
#include <sunspec/exceptions.hpp>

#include <connection/utils.hpp>

using namespace everest::sunspec;

SunspecDevice::SunspecDevice(SunspecDeviceMapping& device_mapping, const int mapped_start, const uint8_t unit_id)
    : device_mapping(device_mapping),
      unit_id(unit_id),
      mapped_start(mapped_start) {}

int SunspecDevice::get_start_address() {
  EVLOG(debug) << "Getting device mapped start.";
  return this->mapped_start;
  EVLOG(debug) << "Done.";
}

const everest::modbus::ModbusClient& SunspecDevice::get_modbus_client() const {
  return this->device_mapping.get_modbus_client();
}

void SunspecDevice::add_model(const uint16_t id, const uint16_t length, const int offset) {
  EVLOG(debug) << "Trying to add model with id " << id << ", length " << length << " and offset " << offset;
  std::unique_ptr<SunspecModel> model = std::make_unique<SunspecModel>(*this, id, length, offset);
  model->initialize_model();
  this->model_name_to_index[ model->get_name() ] = this->models.size();
  EVLOG(debug) << "Added to name map: " << model->get_name() << ": " << this->model_name_to_index[model->get_name()];
  this->models.push_back( std::move(model) );
  EVLOG(debug) << "Added model with id " << id << ", length " << length << " and offset " << offset;
}

const uint8_t& SunspecDevice::get_unit_id() {
  return this->unit_id;
}

const std::vector<std::unique_ptr<SunspecModel>>& SunspecDevice::get_models() {
  return this->models;
}

const json SunspecDevice::get_device_information() const {

  EVLOG(debug) << "Attempting to read device header information...";
  const auto& device_common_model = this->get_model_by_name("common");
  const auto& common_model_points = device_common_model.get_points();

  json device_info;
  std::vector<std::string> info_point_names = {"Mn", "Md", "Vr", "SN"};
  std::string point_label;
  for (const auto& info_point_name : info_point_names ) {
    auto& point_obj = common_model_points.at(info_point_name);
    point_label = point_obj->get_label();
    device_info[point_label] = boost::get<std::string>( point_obj->read() ); 
  }

  return device_info;
}

const SunspecModel& SunspecDevice::get_model_by_name(const std::string& name) const {
  if (!this->model_name_to_index.count(name)) {
    std::stringstream error_msg;
    error_msg << "\nName not present in device model indexes map: " << name;
    EVLOG(error) << error_msg.str();
    throw exceptions::missing_model_name(error_msg.str());
  }
  int index_in_models_list = this->model_name_to_index.at(name);
  return (*this->models[index_in_models_list]);
}

void SunspecDevice::print_name_index_map() const {
  EVLOG(debug) << "Trying to print name index map";
  for (const auto& it : this->model_name_to_index) {
    std::cout << "name: " << it.first << ", index: " << it.second << std::endl;
  }
}
