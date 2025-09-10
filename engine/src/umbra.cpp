#include "Umbra/umbra.hpp"

#include <iostream>
#include <stdexcept>

int umbra::umbra_run(const char* entry_path, const uint8_t* secret, size_t secret_size, int argc, char** argv) {
    std::cout << entry_path << std::endl;
    return 0;
}

void umbra::umbra_fail(const std::string &message) {
    throw std::runtime_error(message);
}

void umbra::umbra_fail(const char *message) {
    throw std::runtime_error(message);
}
