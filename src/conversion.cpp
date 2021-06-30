#include <everest/logging.hpp>

#include <sunspec/conversion.hpp>
#include <sunspec/exceptions.hpp>
#include <sunspec/types.hpp>

#include <connection/utils.hpp>

using namespace everest::sunspec;

const std::map<std::string, types::SunspecTypeInterpreter> conversion::sunspec_interpreters = {
    {"string", conversion::bytevector_to_string},
    {"uint16", conversion::bytevector_to_uint16},
    {"int16", conversion::bytevector_to_int16},
    {"acc16", conversion::bytevector_to_uint16},
    {"enum16", conversion::bytevector_to_uint16},
    {"uint32", conversion::bytevector_to_uint32},
    {"acc32", conversion::bytevector_to_uint32},
    {"enum32", conversion::bytevector_to_uint32},
    {"int32", conversion::bytevector_to_int32},
    {"sunssf", conversion::bytevector_to_int16}
};

std::string conversion::bytevector_to_string(const types::ByteVector& bytevector) {
    EVLOG(debug) << "Converting bytevector to string: " << everest::connection::utils::get_bytes_hex_string(bytevector);
    std::string str(bytevector.begin(), bytevector.end());
    EVLOG(debug) << "To string conversion result: " << str;
    return str;
}

uint16_t conversion::bytevector_to_uint16(const types::ByteVector& bytevector) {

    EVLOG(debug) << "Converting bytevector to uint16: " << everest::connection::utils::get_bytes_hex_string(bytevector);
    if (bytevector.size() > 2) {
        throw exceptions::type_conversion_error("Cannot fit bytevector of size bigger than 2 into uint16.");
    }
    if (bytevector.size() == 0) {
        throw exceptions::type_conversion_error("Cannot convert empty bytevector to uint16.");
    }

    uint16_t result = (bytevector[0] << 8) | bytevector[1];
    EVLOG(debug) << "Conversion to uint16 result: " << result;

    return result;
}

uint32_t conversion::bytevector_to_uint32(const types::ByteVector& bytevector) {

    EVLOG(debug) << "Converting bytevector to uint32: " << everest::connection::utils::get_bytes_hex_string(bytevector);
    if (bytevector.size() > 4) {
        throw exceptions::type_conversion_error("Cannot fit bytevector of size bigger than 4 into uint32.");
    }
    if (bytevector.size() == 0) {
        throw exceptions::type_conversion_error("Cannot convert empty bytevector to uint32.");
    }

    uint32_t result = (bytevector[0] << 24) | (bytevector[1] << 16) | (bytevector[2] << 8) | bytevector[3];
    EVLOG(debug) << "Conversion to uint32 result: " << result;

    return result;
}

int16_t conversion::bytevector_to_int16(const types::ByteVector& bytevector) {

    EVLOG(debug) << "Converting bytevector to int16: " << everest::connection::utils::get_bytes_hex_string(bytevector);
    uint16_t as_uint16 = bytevector_to_uint16(bytevector);
    int16_t result;
    std::memcpy(&result, &as_uint16, sizeof(as_uint16));

    EVLOG(debug) << "Conversion to int16 result: " << result;

    return result;

}

int32_t conversion::bytevector_to_int32(const types::ByteVector& bytevector) {
    EVLOG(debug) << "Converting bytevector to int32: " << everest::connection::utils::get_bytes_hex_string(bytevector);
    uint32_t as_uint32 = bytevector_to_uint32(bytevector);
    int32_t result;
    std::memcpy(&result, &as_uint32, sizeof(as_uint32));

    EVLOG(debug) << "Conversion to int32 result: " << result;

    return result;
}

double conversion::as_numeric(types::SunspecType sunspec_value) {
    if (sunspec_value.type() == typeid(uint16_t))
        return boost::get<uint16_t>(sunspec_value);
    if (sunspec_value.type() == typeid(int16_t))
        return boost::get<int16_t>(sunspec_value);
    if (sunspec_value.type() == typeid(uint32_t))
        return boost::get<uint32_t>(sunspec_value);
    if (sunspec_value.type() == typeid(int32_t))
        return boost::get<int32_t>(sunspec_value);
    throw exceptions::numerical_conversion_error("Value is flagged as numeric but no suitable type was found");
}