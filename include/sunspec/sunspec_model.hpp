// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef SUNSPEC_MODEL_HPP
#define SUNSPEC_MODEL_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <list>
#include <functional>

#include <nlohmann/json.hpp>
#include <modbus/modbus_client.hpp>
#include <sunspec/types.hpp>

// FIXME (aw): no using in public headers!
using json = nlohmann::json;

namespace everest { namespace sunspec {

    class Group;
    class SunspecModel;
    class Point;
    class Symbol;
    class SunspecDevice;

    class Group {
        public:
            Group(SunspecModel& model, json& group_def);
            const std::map<std::string, std::unique_ptr<Point> >& get_points() const;
            Point& get_point_by_name(const std::string& point_name);
            const std::map<std::string, std::unique_ptr<Group>>& get_groups() const;
            const std::map<std::string, std::vector< std::unique_ptr<Group> > >& get_repeating_groups();
            Group& get_repeating_group_by_index(const std::string& group_name, int group_index) const;
            Group& get_subgroup(const std::string& group_name) const;
            const std::string& get_name() const;
            void print_points();

        private:
            Group(const Group&) = delete;
            Group& operator=(const Group&) = delete;
            friend class Point;
            friend class SunspecModel;
            json& group_def;
            SunspecModel& model;
            std::string name;
            std::string type;
            const int offset;
            unsigned int points_len;
            std::map<std::string, std::unique_ptr<Group>> groups;
            std::map<std::string, std::vector< std::unique_ptr<Group> > > repeating_groups;
            std::map<std::string, std::unique_ptr<Point>> points;
            std::unique_ptr<Group> initialize_subgroup(json& group_def);
            std::vector< std::unique_ptr<Group> > make_repeating_groups(json& group_def);
            void parse_group_info();
            void parse_group_points();
            void parse_group_groups();
    };

    class SunspecModel {
        public:
            SunspecModel(SunspecDevice& device, uint16_t model_id, uint16_t length, int offset);
            const everest::modbus::ModbusClient& get_modbus_client() const;
            const std::map<std::string, std::unique_ptr<Point> >& get_points() const;
            const std::map<std::string, std::unique_ptr<Group>>& get_groups() const;
            const std::map<std::string, std::vector<std::unique_ptr<Group>> >& get_repeating_groups() const;
            const std::shared_ptr<Group> get_base_group() const;
            const std::string& get_name() const;
            const uint16_t get_id() const;
            const uint16_t length;
            const uint16_t model_id;
            const int offset;
        
        private:
            SunspecModel(const SunspecModel&) = delete;
            SunspecModel& operator=(const SunspecModel&) = delete;
            friend class SunspecDevice;
            friend class Group;
            friend class Point;
            int offset_count;
            SunspecDevice& device;
            json model_def;
            std::shared_ptr<Group> base_group;
            std::string name;
            types::ModbusReadFunction model_modbus_read_func;
            void initialize_model();
    };

    class Point {
        public:
            Point(SunspecModel& model, Group& group, json& point_def, int size, int offset);
            const std::string& get_name();
            std::string get_label();
            std::string get_description();
            const json& get_point_def();
            const int& get_offset();
            const std::string& get_type();
            const bool& requires_scale_factor();
            int16_t get_scale_factor();
            types::SunspecType read();

        private:
            Point(const Point&) = delete;
            Point& operator=(const Point&) = delete;
            SunspecModel& model;
            Group& group;
            json& point_def;
            std::string name;
            std::string type;
            std::map<std::string, std::unique_ptr<Symbol>> symbols;
            const int size;
            const int offset;
            bool is_numeric;
            bool requires_scale_factor_;
            int16_t scale_factor_value;
            std::function<types::SunspecType()> read_function;
            void parse_symbols();
    };

    class Symbol {
        public:
            Symbol(json& symbol_def, std::string name, int value);

        private:
            Symbol(const Symbol&) = delete;
            Symbol& operator=(const Symbol&) = delete;
            std::string name;
            int value;
            json& symbol_def;
    };

} // namespace sunspec
 } // namespace everest

#endif
