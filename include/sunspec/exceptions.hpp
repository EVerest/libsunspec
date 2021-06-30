#ifndef SUNSPEC_EXCEPTIONS_H
#define SUNSPEC_EXCEPTIONS_H

#include <stdexcept>

namespace everest { namespace sunspec { namespace exceptions {

    class input_error : public std::invalid_argument {
        public:
            input_error(const std::string& what_arg) : std::invalid_argument(what_arg) {}
    };

    class scanning_error : public std::runtime_error {
        public:
            scanning_error(const std::string& what_arg) : std::runtime_error(what_arg) {}
    };

    class type_conversion_error : public std::range_error {
        public:
            type_conversion_error(const std::string& what_arg) : std::range_error(what_arg) {}
    };

    class scaling_error : public std::range_error {
        public:
            scaling_error(const std::string& what_arg) : std::range_error(what_arg) {}
    };

    class point_read_error : public std::runtime_error {
        public:
            point_read_error(const std::string& what_arg) : std::runtime_error(what_arg) {}
    };

    class missing_model_name : public std::out_of_range {
        public:
            missing_model_name(const std::string& what_arg) : std::out_of_range(what_arg) { }
    };

    class bad_query_string : public std::invalid_argument {
        public:
            bad_query_string(const std::string& what_arg) : std::invalid_argument(what_arg) { }
    };

    class model_access_error : public std::out_of_range {
        public:
            model_access_error(const std::string& what_arg) : std::out_of_range(what_arg) { }
    };

    class group_access_error : public std::out_of_range {
        public:
            group_access_error(const std::string& what_arg) : std::out_of_range(what_arg) { }
    };

    class point_access_error : public std::out_of_range {
        public:
            point_access_error(const std::string& what_arg) : std::out_of_range(what_arg) { }
    };

    class numerical_conversion_error : public type_conversion_error {
        public:
            numerical_conversion_error(const std::string& what_arg) : type_conversion_error(what_arg) {}
    };


}  // namespace exceptions
 }  // namespace sunspec
  }; // namespace everest

#endif
