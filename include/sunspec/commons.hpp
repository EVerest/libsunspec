// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef COMMONS_HPP
#define COMMONS_HPP

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <boost/variant.hpp>
#include <cstdint>

namespace everest { namespace sunspec { namespace commons {

    //! Folder to look for standard models defined by Sunspec.
    constexpr char JSON_MODELS_BASE_FOLDER[] = "dist/sunspec/models";
    //! Base MODBUS addresses to look for while scanning for Sunspec-compliant devices.
    constexpr std::array<int, 3> BASE_REGISTER_ADDRESSES{40000, 0, 50000};
    //! Sunspec identifier. Always precedes a the mapping of a Sunspec-compliant device.
    constexpr char SUNS_IDENTIFIER[] = "SunS";
    //! Default model info block length.
    constexpr size_t MODEL_INFO_LEN = 2;
    //! Default model info block length in bytes.
    constexpr size_t MODEL_INFO_LEN_BYTES = MODEL_INFO_LEN * 2;
    //! Common Sunspec Model ID
    constexpr uint16_t COMMON_MODEL_ID = 1;
    //! Zero length (final) model ID, as defined by the Sunspec standard.
    constexpr uint16_t ZERO_LENGTH_MODEL_ID = 0xFFFF;
    //! Length of the zero-length model.
    constexpr uint16_t ZERO_LENGTH_MODEL_LENGTH = 0;
    //! Constant for the undefined value.
    constexpr int16_t UNDEFINED_SF_VALUE = 0x8000;
    //! Sunspec types that are strictly numeric
    const std::set<std::string> NUMERIC_TYPES = {"uint16", "int16", "uint32", "int32"};

} // namespace commons
 } // namespace sunspec
   } // namespace everest

#endif
