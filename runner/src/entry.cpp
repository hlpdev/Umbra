#include "Umbra/boot.hpp"
#include "Umbra/magics.hpp"
#include "Umbra/pak.hpp"
#include "Umbra/umbra.hpp"

#include <filesystem>
#include <fstream>
#include <toml++/toml.hpp>

#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

static std::filesystem::path executable_path() {
#ifdef _WIN32
  char buffer[MAX_PATH];
  const DWORD size = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
  return std::filesystem::path(std::string(buffer, buffer + size));
#elif __APPLE__
  uint32_t size = 0; _NSGetExecutablePath(nullptr, &size);
  std::string temp(size, '\0'); _NSGetExecutablePath(temp.data(), &size);
  return std::filesystem::canonical(temp);
#else
  char buffer[4096]; ssize_t n = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (n > 0) { buffer[n] = 0; return std::filesystem::path(buffer); }
  return std::filesystem::canonical("/proc/self/exe");
#endif
}

static std::vector<uint8_t> read_embedded_secret(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    umbra::umbra_fail("Runner: open self failed");
  }

  file.seekg(-static_cast<std::streamoff>(sizeof(umbra::EmbeddedFooter)), std::ios::end);

  umbra::EmbeddedFooter footer{};
  file.read(reinterpret_cast<char*>(&footer), sizeof(footer));
  if (!file) {
    umbra::umbra_fail("Runner: footer read failed");
  }

  if (std::memcmp(footer.magic, umbra::SECRET_KEY_MAGIC, umbra::SECRET_KEY_MAGIC_LEN) != 0 || footer.version != UMBRA_VERSION) {
    umbra::umbra_fail("Runner: invalid footer");
  }

  if (footer.key_length == 0 || footer.key_length > 1 << 20) {
    umbra::umbra_fail("Runner: invalid key length");
  }

  std::vector<uint8_t> key(footer.key_length);
  file.seekg(-static_cast<std::streamoff>(sizeof(umbra::EmbeddedFooter) + footer.key_length), std::ios::end);
  file.read(reinterpret_cast<char*>(key.data()), footer.key_length);
  if (!file) {
    umbra::umbra_fail("Runner: key read failed");
  }

  return key;
}

int main(int argc, char** argv) {
  const std::filesystem::path executable = executable_path();
  const std::filesystem::path dir = executable.parent_path();
  const std::filesystem::path config_path = dir / "cfg.pak";
  const std::filesystem::path source_path = dir / "src.pak";
  const std::filesystem::path assets_path = dir / "assets.pak";

  std::vector<uint8_t> key = read_embedded_secret(executable);

  auto config_reader = std::make_unique<umbra::PakReader>(config_path, key);

  constexpr auto config_virtual_path = "umbra.toml";
  if (!config_reader->contains(config_virtual_path)) {
    umbra::umbra_fail("Runner: umbra.toml not found in cfg.pak");
  }

  const std::vector<uint8_t> config_buffer = config_reader->read(config_virtual_path);

  const toml::table config = toml::parse(std::string_view(reinterpret_cast<const char*>(config_buffer.data()), config_buffer.size()));
  const std::string entry = config["entry"].value_or(std::string{});
  if (entry.empty()) {
    umbra::umbra_fail("Runner: entry missing in project configuration");
  }

  return umbra::umbra_run(entry.c_str(), key.data(), key.size(), argc, argv);
}