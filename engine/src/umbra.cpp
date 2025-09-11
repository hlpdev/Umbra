#include "Umbra/umbra.hpp"

#include <iostream>

#include "Umbra/config.hpp"
#include "Umbra/fs-vfs.hpp"
#include "Umbra/pak-vfs.hpp"
#include "Umbra/umbra_exception.hpp"
#include "Umbra/vfs.hpp"

#include <vector>

using namespace std::string_literals;

static std::string sanitize_alphanumeric(const std::string& str) {
  std::string out; out.reserve(str.size());

  for (const uint8_t c : str) {
    if (std::isalnum(c)) {
      out.push_back(c);
    }
  }

  return out;
}

std::filesystem::path user_data_root() {
#if defined(_WIN32)
  if (const char* p = std::getenv("APPDATA")) return p;
  if (const char* u = std::getenv("USERPROFILE")) return std::filesystem::path(u) / "AppData/Roaming";
  return {};
#elif defined(__APPLE__)
  if (const char* h = std::getenv("HOME")) return std::filesystem::path(h) / "Library/Application Support";
  return {};
#else
  if (const char* x = std::getenv("XDG_DATA_HOME")) return x;
  if (const char* h = std::getenv("HOME")) return std::filesystem::path(h) / ".local/share";
  return {};
#endif
}

struct EngineState {
  umbra::VFS vfs;
  umbra::Config config;
};

int umbra::umbra_run(const char* entry_path, const uint8_t* secret, size_t secret_size, int argc, char** argv) try {
  if (!entry_path || !*entry_path) {
    umbra_fail("Umbra: entry path is empty");
  }

  if (!secret || !secret_size) {
    umbra_fail("Umbra: secret is empty");
  }

  std::vector<uint8_t> key(secret, secret + secret_size);

  EngineState state{};

  state.vfs.mount(
    "cfg://",
    std::make_unique<VFSPakMount>(
      "cfg.pak",
      key,
      vfs::permissions::READ
    )
  );

  if (!state.vfs.exists("cfg://umbra.toml")) {
    umbra_fail("Umbra: cfg.pak does not contain umbra.toml");
  }

  state.config = load_config(state.vfs.read("cfg://umbra.toml"));

  state.vfs.mount(
    "src://",
    std::make_unique<VFSPakMount>(
      "src.pak",
      key,
      vfs::permissions::EXECUTE | vfs::permissions::READ | vfs::permissions::LIST
    )
  );

  state.vfs.mount(
    "assets://",
    std::make_unique<VFSPakMount>(
      "ass.pak",
      key,
      vfs::permissions::READ | vfs::permissions::LIST
    )
  );

  state.vfs.mount(
    "data://",
    std::make_unique<VFSFSMount>(
      "data",
      vfs::permissions::READ | vfs::permissions::WRITE | vfs::permissions::CREATE | vfs::permissions::REMOVE | vfs::permissions::LIST
    )
  );

  state.vfs.mount(
    "user://",
    std::make_unique<VFSFSMount>(
      user_data_root() / sanitize_alphanumeric(state.config.name),
      vfs::permissions::READ | vfs::permissions::WRITE | vfs::permissions::CREATE | vfs::permissions::REMOVE | vfs::permissions::LIST
    )
  );

  if (!state.vfs.exists("src://"s + entry_path)) {
    umbra_fail("Umbra: entry script not found");
  }

  // TODO

  return 0;
} catch (const UmbraException&) {
  return 1;
} catch (const std::exception& e) {
  std::cerr << e.what() << '\n';
  return 1;
}

void umbra::umbra_fail(const std::string &message) {
  std::cerr << message << '\n';
  throw UmbraException(message);
}

void umbra::umbra_fail(const char *message) {
  std::cerr << message << '\n';
  throw UmbraException(message);
}
