#include "Umbra/config.hpp"

#include <fmt/format.h>
#include <toml++/toml.hpp>

std::filesystem::path umbra::Config::out_dir() const {
  return std::filesystem::current_path() / name;
}

UMBRA_API umbra::Config umbra::load_config(const std::filesystem::path &project_dir) {
  std::filesystem::path root = project_dir.empty() ? std::filesystem::current_path() : project_dir;
  root = absolute(root);

  const std::filesystem::path config_path = root / "umbra.toml";
  if (!exists(config_path)) {
    umbra_fail("Config: umbra.toml not found at: " + config_path.string());
  }

  toml::table config_toml;
  try {
    config_toml = toml::parse_file(config_path.string());
  } catch (const toml::parse_error& e) {
    toml::source_region src = e.source();
    umbra_fail(fmt::format("Config: toml parse error in {} ({}:{}): {}", config_path.string(), src.begin.line, src.begin.column, e.description()));
  }

  auto require_string = [&](std::string_view key) -> std::string {
    const toml::node_view<toml::node> value = config_toml[key];
    if (!value || !value.is_string()) {
      umbra_fail(fmt::format("Config: required field not found: {}", key));
    }

    return std::string{ *value.as_string() };
  };

  Config config{};
  config.root_dir = root;
  config.config_file = config_path;
  config.name = config_toml["name"].value_or(root.filename().string());
  config.entry = require_string("entry");

  std::string assets_relative = config_toml["assets_dir"].value_or("assets");
  std::string source_relative = config_toml["source_dir"].value_or("source");

  config.assets_dir = absolute(root / assets_relative);
  config.source_dir = absolute(root / source_relative);

  if (!exists(config.assets_dir)) {
    umbra_fail(fmt::format("Config: assets directory not found: {}", config.assets_dir.string()));
  }

  if (!exists(config.source_dir)) {
    umbra_fail(fmt::format("Config: source directory not found: {}", config.source_dir.string()));
  }

  if (!exists(config.config_file)) {
    umbra_fail(fmt::format("Config: config file not found: {}", config.config_file.string()));
  }

  return config;
}
