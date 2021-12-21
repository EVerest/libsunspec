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
#ifndef SUNSPEC_CONVERSION_HPP
#define SUNSPEC_CONVERSION_HPP

#include <map>

#include <sunspec/types.hpp>

namespace everest { namespace sunspec { namespace conversion {

    std::string bytevector_to_string(const types::ByteVector& bytevector);
    uint16_t bytevector_to_uint16(const types::ByteVector& bytevector);
    int16_t bytevector_to_int16(const types::ByteVector& bytevector);
    uint32_t bytevector_to_uint32(const types::ByteVector& bytevector);
    int32_t bytevector_to_int32(const types::ByteVector& bytevector);
    double as_numeric(types::SunspecType sunspec_value);

    extern const std::map<std::string, types::SunspecTypeInterpreter> sunspec_interpreters;

} // namespace conversion
 } // namespace sunspec
  }; // namespace everest

#endif