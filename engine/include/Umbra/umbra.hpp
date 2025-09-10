#pragma once

#include "magics.hpp"

#include <stdint.h>
#include <string>

namespace umbra {

    UMBRA_API int umbra_run(const char* entry_path, const uint8_t* secret, size_t secret_size, int argc, char** argv);

    [[noreturn]] UMBRA_API void umbra_fail(const std::string& message);
    [[noreturn]] UMBRA_API void umbra_fail(const char* message);

}