// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <fstream>
#include <sstream>

#include <everest/logging.hpp>
#include <nlohmann/json.hpp>

#include <sunspec/sunspec_model.hpp>
#include <sunspec/sunspec_device.hpp>
#include <sunspec/utils.hpp>
#include <sunspec/exceptions.hpp>
#include <sunspec/conversion.hpp>

using namespace everest::sunspec;
using json = nlohmann::json;

const std::map<std::string, std::unique_ptr<Point> >& Group::get_points() const {
    return this->points;
}

Point& Group::get_point_by_name(const std::string& point_name) {
    if (!this->points.count(point_name)) {
        std::stringstream error_msg;
        error_msg << "Point with name " << point_name << " not contained in group with name " << this->name;
        EVLOG(error) << error_msg.str();
        throw exceptions::point_access_error(error_msg.str());
    }
    return *(this->points.at(point_name));
}

const std::map<std::string, std::unique_ptr<Group>>& Group::get_groups() const {
    return this->groups;
}

const std::map<std::string, std::vector< std::unique_ptr<Group> > >& Group::get_repeating_groups() {
    return this->repeating_groups;
}

void Group::parse_group_points() {
    for (auto& point_def : this->group_def["points"]) {
        EVLOG(debug) << point_def["name"];
        this->points[ point_def["name"] ] = std::make_unique<Point>(this->model, (*this), point_def, point_def["size"], this->model.offset_count);
        EVLOG(debug) << "Created point with name " << this->points[ point_def["name"] ]->get_name() << " - Requires SF: " << this->points[ point_def["name"] ]->requires_scale_factor();
        this->model.offset_count += point_def["size"].get<int>();
    }
}

std::unique_ptr<Group> Group::initialize_subgroup(json& group_def) {
    EVLOG(debug) << "Parsing group " << group_def["name"];
    auto group_ptr = std::make_unique<Group>(this->model, group_def);
    group_ptr->parse_group_info();
    return group_ptr;
}

std::vector< std::unique_ptr<Group> > Group::make_repeating_groups(json& group_def) {
    std::vector< std::unique_ptr<Group> > groups;
    for (int i = 0 ; i < group_def["count"] ; i++) {
        std::unique_ptr<Group> group = this->initialize_subgroup(group_def);
        groups.push_back( std::move(group) );
    }
    return groups;
}

void Group::parse_group_groups() {
    for (auto& group_def : this->group_def["groups"]) {
        if (!utils::json_contains(group_def, "count"))
            this->groups[ group_def["name"] ] = this->initialize_subgroup(group_def);
        else
            this->repeating_groups[ group_def["name"] ] = this->make_repeating_groups(group_def);
    }
}

void Group::parse_group_info() {
    if (utils::json_contains(this->group_def, "points")) {
        EVLOG(debug) << "Reading group \"" << this->name << "\" points.";
        this->parse_group_points();
    }
    if (utils::json_contains(this->group_def, "groups"))  {
        EVLOG(debug) << "Reading group \"" << this->name << "\" groups.";
        this->parse_group_groups();
    }
}

Group::Group(SunspecModel& model, json& group_def) 
    : model(model), group_def(group_def),
      offset(model.offset), name(group_def["name"]),
      type(group_def["type"]), points_len(0) { }

void Group::print_points() {
    if (this->points.size() > 0) {
        for (const auto& point : this->points) {
            const auto& point_name = point.first;
            std::cout << point_name << " ";
        }
    }
    std::cout << std::endl;
}

Group& Group::get_subgroup(const std::string& group_name) const {
    if (!this->groups.count(group_name)) {
        std::stringstream error_msg;
        error_msg << "Subgroup " << group_name << " does not exist inside group " << this->name;
        EVLOG(error) << error_msg.str();
        throw exceptions::group_access_error(error_msg.str());
    }
    return *(this->groups.at(group_name));
}

Group& Group::get_repeating_group_by_index(const std::string& group_name, int group_index) const {

    // Checking if group with 'group_name' exists as a subgroup inside this group
    if (!this->repeating_groups.count(group_name)) {
        std::stringstream error_msg;
        error_msg << "Subgroup " << group_name << " does not exist as a repeating group inside group " << this->name;
        EVLOG(error) << error_msg.str();
        throw exceptions::group_access_error(error_msg.str());
    }

    // If so, check the validity of the index
    auto& repeating_group = this->repeating_groups.at(group_name);
    if (group_index < 0 || group_index >= repeating_group.size()) {
        std::stringstream error_msg;
        error_msg << "Repeating group with name " << group_name << "smaller than size specified by index " << group_index;
        EVLOG(error) << error_msg.str();
        throw exceptions::group_access_error(error_msg.str());
    }
    return *(repeating_group[group_index]);

}

const std::string& Group::get_name() const {
    return this->name;
}
 
const uint16_t SunspecModel::get_id() const {
    return this->model_id;
}

SunspecModel::SunspecModel(SunspecDevice& device, const uint16_t model_id, const uint16_t length, const int offset)
    : device(device), model_id(model_id), length(length),
      offset(offset), offset_count(offset) {

    this->model_def = utils::json_from_model_id(model_id);

}

void SunspecModel::initialize_model() {
    
    this->base_group = std::make_shared<Group>(*this, this->model_def["group"]);
    this->base_group->parse_group_info();
    this->name = this->base_group->group_def["name"];
    // Making read function that will serve to read points from this model
    this->model_modbus_read_func = utils::make_modbus_read_function(this->get_modbus_client(), this->device.get_unit_id());
}

const std::string& SunspecModel::get_name() const {
    return this->name;
}

const std::map<std::string, std::unique_ptr<Point> >& SunspecModel::get_points() const {
    return this->base_group->get_points();
}

const std::map<std::string, std::unique_ptr<Group> >& SunspecModel::get_groups() const {
    return this->base_group->get_groups();
}

const std::map<std::string, std::vector<std::unique_ptr<Group>> >& SunspecModel::get_repeating_groups() const {
    return this->base_group->get_repeating_groups();
}

const std::shared_ptr<Group> SunspecModel::get_base_group() const {
    return this->base_group;
}

const everest::modbus::ModbusClient& SunspecModel::get_modbus_client() const {
    return this->device.get_modbus_client();
}

Point::Point(SunspecModel& model, Group& group, json& point_def, const int size, const int offset)
    : model(model), group(group),
      point_def(point_def), size(size),
      offset(offset), name(point_def["name"]),
      type(point_def["type"]), requires_scale_factor_(false),
      scale_factor_value(commons::UNDEFINED_SF_VALUE) {

    EVLOG(debug) << "Creating point with name " << point_def["name"] << ", offset " << offset << ", size " << size << " inside model with name " << this->model.base_group->group_def["name"] << " and ID " << this->model.model_def["id"];
    
    // Checking for symbols in this point
    if (utils::json_contains(point_def, "symbols")) {
        this->parse_symbols();
    }

    // Checking if this point requires the application of a scale factor
    if (utils::json_contains(point_def, "sf")) {
        this->requires_scale_factor_ = true;
    }

    // Checking if this point is a numeric type
    this->is_numeric = commons::NUMERIC_TYPES.count(this->type);

}

int16_t Point::get_scale_factor() {

    EVLOG(debug) << "Attempting to read scale factor for point " << this->name << " with address offset " << this->offset;

    if (!this->requires_scale_factor_)
        throw exceptions::scaling_error("Trying to read scale factor for a point that doesn't require one.");
    
    if (this->scale_factor_value != commons::UNDEFINED_SF_VALUE)
        return this->scale_factor_value;

    const auto& scale_factor_point_name = this->point_def["sf"];

    try {
        const auto& scale_factor_point = this->group.get_points().at(scale_factor_point_name);
        EVLOG(debug) << "Scale factor of point " << this->name << " found at offset " << scale_factor_point->offset;
        int scale_factor_as_int = boost::get<int16_t>( scale_factor_point->read() );
        this->scale_factor_value = scale_factor_as_int;
    }
    catch (const std::out_of_range& oor) {
        std::stringstream error_msg;
        error_msg << "Out of range error: " << oor.what() << ". The scale factor of point " << this->name << " is not contained within the same group.";
        EVLOG(error) << error_msg.str();
        exceptions::point_read_error(error_msg.str());
    }
    return this->scale_factor_value;
}

void Point::parse_symbols() {
    EVLOG(debug) << "Parsing symbols for point " << this->name;
    for (auto& symbol : this->point_def["symbols"]) {
        this->symbols[ symbol["name"] ] = std::make_unique<Symbol>(symbol, symbol["name"], symbol["value"]);
    }
}

types::SunspecType Point::read() {
    const types::ByteVector& bytevector = this->model.model_modbus_read_func(this->offset, this->size);
    try {
        types::SunspecTypeInterpreter conversion_function = conversion::sunspec_interpreters.at(this->type);
        types::SunspecType converted_value = conversion_function(bytevector);
        EVLOG(debug) << "Point::read() - Conversion function result: " << converted_value;
        if (this->is_numeric)
            return conversion::as_numeric(converted_value);
        return converted_value;
    }
    catch (const std::out_of_range& oor) {
        std::stringstream error_msg;
        error_msg << "Out of range error: " << oor.what() << ". No type conversion function found for type " << this->type << ". Point name: " << this->name;
        EVLOG(error) << error_msg.str();
        throw exceptions::point_read_error(error_msg.str());
    }
}

const std::string& Point::get_name() {
    return this->name;
}

std::string Point::get_label() {
    return this->point_def["label"];
}

std::string Point::get_description() {
    return this->point_def["desc"];
}

const json& Point::get_point_def() {
    return this->point_def;
}

const std::string& Point::get_type() {
    return this->type;
}

const int& Point::get_offset() {
    return this->offset;
}

const bool& Point::requires_scale_factor() {
    return this->requires_scale_factor_;
}

Symbol::Symbol(json& symbol_def, std::string name, int value) 
    : symbol_def(symbol_def), name(name),
      value(value) {
    
    EVLOG(debug) << "Creating symbol with name " << name << " and value " << value;

}
