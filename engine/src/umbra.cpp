#include "Umbra/umbra.hpp"

#include <stdexcept>

int umbra::umbra_run(const int8_t* entry_path, const uint8_t* secret, size_t secret_size) {

    return 0;
}

void umbra::umbra_fail(const std::string &message) {
    throw std::runtime_error(message);
}

void umbra::umbra_fail(const char *message) {
    throw std::runtime_error(message);
}
