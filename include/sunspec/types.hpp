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
#ifndef SUNSPEC_TYPES_HPP
#define SUNSPEC_TYPES_HPP

#include <string>
#include <vector>
#include <functional>
#include <boost/variant.hpp>

namespace everest { namespace sunspec { namespace types {

    using SunspecType = boost::variant<uint16_t, int16_t, uint32_t, int32_t, double, std::string>;
    using ByteVector = std::vector<uint8_t>;
    using ModbusReadFunction = std::function<ByteVector(uint16_t first_register_address, uint16_t num_registers_to_read)>;
    using SunspecTypeInterpreter = std::function<SunspecType(const ByteVector& bytevector)>;

} // namespace types
 } // namespace sunspec
  }; // namespace everest

#endif
