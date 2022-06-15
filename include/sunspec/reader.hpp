// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef QUERY_LANGUAGE_HPP
#define QUERY_LANGUAGE_HPP

#include <string>
#include <regex>
#include <iostream>

#include <everest/logging.hpp>

#include <sunspec/sunspec_device_mapping.hpp>
#include <sunspec/sunspec_model.hpp>
#include <sunspec/utils.hpp>
#include <sunspec/exceptions.hpp>

#define UNDEFINED_GROUP_INDEX -1

namespace everest { namespace sunspec {

    const std::regex QUERY_PATTERN = std::regex("<(.+?)>(?:::|$)");
    const std::regex GROUP_INDEX_PATTERN = std::regex("(.*)\\[(\\d+)\\]");

    //! SunspecReader class - class used to query the given Sunspec device.
    /*!
        Given an input SunspecDeviceMapping object and a query string, this class is responsible for parsing a string and looking
        for the Point object that corresponds to the one given in the input string 'query'. It provides an easier interface
        to deal with querying Sunspec Points that are nested or contained within arrays.
    */

    template <typename PointType>
    class SunspecReader {
        public:
            /*!
            *   Create a new SunspecReader object with the query defined by the query string 'query', to be 
            *   applied to the SunspecDeviceMapping object passed as the 'device_mapping' argument.
            *   Since all Point objects return a boost::variant, a type has to be informed to this class,
            *   so that the value in the correct type is returned. This allows the user to read the value of points
            *   of any type (strings, uints, ints, enum values, etc.)
            */
            SunspecReader(const std::string& query, const SunspecDeviceMapping& device_mapping, bool initialize=true)
                : device_mapping(device_mapping) {

                if (!this->is_query_valid(query)) {
                    throw exceptions::bad_query_string("Query string does not match the required format.");
                }
                const auto& query_contents = this->parse_query_arguments(query);

                // Simple argument count validation
                this->validate_query_contents(query_contents);
                this->parse_query_contents(query_contents);

                if (initialize) {
                    this->init();
                }

            }

            void init() {
                this->query_device = &this->find_query_device();
                this->query_point = &this->find_query_point();
            }

            /*!
            *   Prints the summary of the parsed query. This can be used to check if everything went well with your query.
            */
            void print_query_summary() {
                std::cout << "device index: " << this->device_index << std::endl;
                std::cout << "model name: " << this->model_name << std::endl;
                for (const auto& group_name_and_index : this->groupnames_and_indexes) {
                    std::cout << "inner group name: " << std::get<0>(group_name_and_index) << ", index:" << std::get<1>(group_name_and_index) << std::endl;
                }
                std::cout << "point name: " << this->point_name << std::endl;
            }

            /*!
            *   Returns the query device information (model, manufacturer, etc). The query device is the one identified as the first argument in the point query
            */
            json get_query_device_information() {
                if (!this->information_cache.count("query_device"))
                    this->information_cache["query_device"] = this->query_device->get_device_information();
                return this->information_cache["query_device"];
            }

            /*!
            *   Returns the information (model, manufacturer, etc) of all devices contained in this device mapping.
            */
            json get_devices_information() {

                if (this->information_cache.count("query_devices"))
                    return this->information_cache["query_device"];

                json formatted_devices_info;
                int device_counter = 0;
                const auto& devices_info = this->device_mapping.get_devices_information();
                for (const auto& device_info : devices_info) {
                    formatted_devices_info[device_counter] = device_info;
                    device_counter++;
                }
                this->information_cache["query_device"] = formatted_devices_info;
                return formatted_devices_info;
            }

            /*!
            *   Reads the value of the Sunspec Point value defined in the query.
            */
            PointType read() {
                PointType point_value = boost::get<PointType>(this->query_point->read());
                EVLOG_debug << "Reader::read() - Read point value: " << point_value;
                this->cached_value = point_value;
                return point_value;
            }

            /*!
            *   The get method works almost in the same way as the read() method. However, it allows the user to access a cached value.
            *   This can be useful in the situation where the user doesn't want to query the client again, just reading the value that was 
            *   last read.
            */
            PointType get(bool cached=false) {
                if (cached)
                    return this->cached_value;
                return this->read();
            }

            /*!
            *   Since some values of Sunspec Point's need to be combined with a scale factor, this method allows the user
            *   to directly query the scale factor as well.
            */
            int16_t get_scale_factor() {
                return this->query_point->get_scale_factor();
            }

        private:
            int device_index;
            std::string model_name;
            std::vector<std::tuple<std::string, int>> groupnames_and_indexes;
            std::string point_name;
            const SunspecDeviceMapping& device_mapping;
            const SunspecDevice* query_device;
            Point* query_point;
            PointType cached_value;
            std::map<std::string, json> information_cache;

            bool is_query_valid(const std::string& query) {
                return std::regex_match(query, QUERY_PATTERN);
            }

            std::vector<std::tuple<std::string, int>> parse_inner_groups_names(const std::vector<std::string>& inner_groups_names) {
                std::smatch s;
                std::vector<std::tuple<std::string, int>> group_names_and_indexes;
                for (const std::string& group_name : inner_groups_names) {
                    group_names_and_indexes.push_back( parse_group_indexes(group_name) );
                }
                return group_names_and_indexes;
            }

            std::tuple<std::string, int> parse_group_indexes(const std::string& group_string) {

                if (!this->group_has_index(group_string)) {
                    return std::make_tuple(group_string, UNDEFINED_GROUP_INDEX);
                }

                std::string group_name;
                std::smatch s;
                int group_index;

                std::regex_search(group_string, s, GROUP_INDEX_PATTERN);
                group_name = s.str(1);
                group_index = std::stoi(s.str(2));
                return std::make_tuple(group_name, group_index);

            }

            bool group_has_index(const std::string& group_string) {
                return std::regex_match(group_string, GROUP_INDEX_PATTERN);
            }

            void parse_query_contents(const std::vector<std::string>& query_contents) {

                this->device_index = std::stoi( query_contents[0] );
                this->model_name = query_contents[1];
                this->point_name = query_contents.back();
                auto inner_groups_names = std::vector<std::string>(query_contents.begin() + 2, query_contents.end() - 1);

                // Parse inner group names here
                this->groupnames_and_indexes = this->parse_inner_groups_names(inner_groups_names);

            }

            void validate_query_contents(const std::vector<std::string>& query_contents) {
                if (query_contents.size() < 3)
                    throw exceptions::bad_query_string("Not enough arguments for query string. (Minimum of 3 required).");
            }

            std::vector<std::string> parse_query_arguments(std::string query) {    
                std::vector<std::string> matches;
                for (std::smatch s ; std::regex_search(query, s, QUERY_PATTERN) ; query = s.suffix()) {
                    matches.push_back(s.str(1));
                }
                return matches;
            }

            const SunspecDevice& find_query_device() const {
                return this->device_mapping.get_device_by_index(this->device_index);
            }

            Point& find_query_point() {
                const auto& device = this->device_mapping.get_device_by_index(this->device_index);
                const auto& model = device.get_model_by_name(this->model_name);
                Group* group_ptr = model.get_base_group().get();
                if (this->groupnames_and_indexes.size() > 0) {
                    std::string group_name;
                    int group_index;
                    for (const auto& group_name_and_index : this->groupnames_and_indexes) {
                        group_name = std::get<0>(group_name_and_index);
                        group_index = std::get<1>(group_name_and_index);
                        if (group_index == UNDEFINED_GROUP_INDEX) {
                            group_ptr = &group_ptr->get_subgroup(group_name);
                        }
                        else {
                            group_ptr = &group_ptr->get_repeating_group_by_index(group_name, group_index);
                        }
                    }
                }
                return group_ptr->get_point_by_name(this->point_name);
            }

    };

} // namespace sunspec 
 }  // namespace everest

#endif
