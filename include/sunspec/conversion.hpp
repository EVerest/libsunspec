// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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
    float bytevector_to_float32(const types::ByteVector& bytevector);
    double as_numeric(types::SunspecType sunspec_value);

    extern const std::map<std::string, types::SunspecTypeInterpreter> sunspec_interpreters;

} // namespace conversion
 } // namespace sunspec
  }; // namespace everest

#endif
