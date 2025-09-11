#pragma once

#include "Umbra/umbra.hpp"

#include <filesystem>
#include <string>

namespace umbra {

  struct Config final {
    std::string name;
    std::filesystem::path root_dir;
    std::filesystem::path config_file;
    std::filesystem::path assets_dir;
    std::filesystem::path source_dir;
    std::string entry;

    std::filesystem::path out_dir() const;
  };

  UMBRA_API Config load_config(const std::filesystem::path& project_dir);
  UMBRA_API Config load_config(const std::vector<uint8_t>& data);
}