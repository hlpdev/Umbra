#pragma once

#include "magics.hpp"

#include <stdint.h>
#include <string>

namespace umbra {

    UMBRA_API int umbra_run(const int8_t* entry_path, const uint8_t* secret, size_t secret_size);

    [[noreturn]] UMBRA_API void umbra_fail(const std::string& message);
    [[noreturn]] UMBRA_API void umbra_fail(const char* message);

}