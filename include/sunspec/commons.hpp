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
    const std::string JSON_MODELS_BASE_FOLDER = "sunspec/models";
    //! Base MODBUS addresses to look for while scanning for Sunspec-compliant devices.
    const std::array<int, 3> BASE_REGISTER_ADDRESSES{40000, 0, 50000};
    //! Sunspec identifier. Always precedes a the mapping of a Sunspec-compliant device.
    const std::string SUNS_IDENTIFIER = "SunS";
    //! Default model info block length.
    const size_t MODEL_INFO_LEN = 2;
    //! Default model info block length in bytes.
    const size_t MODEL_INFO_LEN_BYTES = MODEL_INFO_LEN * 2;
    //! Common Sunspec Model ID
    const uint16_t COMMON_MODEL_ID = 1;
    //! Zero length (final) model ID, as defined by the Sunspec standard.
    const uint16_t ZERO_LENGTH_MODEL_ID = 0xFFFF;
    //! Length of the zero-length model.
    const uint16_t ZERO_LENGTH_MODEL_LENGTH = 0;
    //! Constant for the undefined value.
    const int16_t UNDEFINED_SF_VALUE = 0x8000;
    //! Sunspec types that are strictly numeric
    const std::set<std::string> NUMERIC_TYPES = {"uint16", "int16", "uint32", "int32"};

} // namespace commons
 } // namespace sunspec
   } // namespace everest

#endif