#include <filesystem>
#include <fstream>
#include <iostream>
#include <sodium.h>
#include <Umbra/pak.hpp>

#include "Umbra/boot.hpp"
#include "Umbra/config.hpp"

#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

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

static std::filesystem::path executable_parent_path() {
#ifdef _WIN32
  char buf[MAX_PATH];
  GetModuleFileNameA(nullptr, buf, MAX_PATH);
  return std::filesystem::path(buf).parent_path();
#elif __APPLE__
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  std::string temp(size, '\0');
  _NSGetExecutablePath(temp.data(), &size);
  return std::filesystem::path(temp).parent_path();
#else
  char buf[4096];
  ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if (size > 0) {
    buf[size] = 0;
    return std::filesystem::path(buf).parent_path();
  }
  return std::filesystem::current_path();
#endif
}

static void ensure_dir(const std::filesystem::path& path) {
  std::error_code ec;
  create_directories(path, ec);
  if (ec) {
    throw std::runtime_error("CLI: directory creation failed ("s + ec.message() + ")");
  }
}

static void copy_shared_libs(const std::filesystem::path& source_dir, const std::filesystem::path& dest_dir) {
#ifdef _WIN32
  const char* extensions[] = { ".dll" };
#elif __APPLE__
  const char* extensions[] = { ".dylib" };
#else
  const char* extensions[] = { ".so" };
#endif

  ensure_dir(dest_dir);
  for (const auto& entry : std::filesystem::directory_iterator(source_dir)) {
    if (!entry.is_regular_file()) continue;

    std::string extension = entry.path().extension().string();
    for (const char* want : extensions) {
      if (extension == want) {
        copy_file(entry.path(), dest_dir / entry.path().filename(), std::filesystem::copy_options::overwrite_existing);
      }
    }
  }
}

static std::vector<uint8_t> generate_secret() {
  if (sodium_init() < 0) {
    umbra::umbra_fail("CLI: failed to init sodium");
  }

  std::vector<uint8_t> secret(crypto_secretstream_xchacha20poly1305_KEYBYTES);
  randombytes_buf(secret.data(), secret.size());
  return secret;
}

static void append_secret_trailer(const std::filesystem::path& runner_path, const std::vector<uint8_t>& secret) {
  if (secret.empty()) {
    umbra::umbra_fail("CLI: secret is empty");
  }

  std::ofstream file(runner_path, std::ios::binary | std::ios::app);
  if (!file) {
    umbra::umbra_fail("CLI: could not open runner for secret appendage");
  }

  const umbra::EmbeddedFooter footer = umbra::make_footer(static_cast<uint32_t>(secret.size()));
  file.write(reinterpret_cast<const char*>(secret.data()), static_cast<std::streamsize>(secret.size()));
  file.write(reinterpret_cast<const char*>(&footer), sizeof(footer));

  if (!file) {
    umbra::umbra_fail("CLI: could not write secret footer to runner");
  }
}

#ifdef _WIN32
static constexpr const char* RUNNER_NAMES[] = { "umbra-runner.exe" };
#else
static constexpr const char* RUNNER_NAMES[] = { "umbra-runner" };
#endif

static std::filesystem::path find_runner(const std::filesystem::path& cli_dir) {
  for (const char* name : RUNNER_NAMES) {
    std::filesystem::path path = absolute(cli_dir / name);
    if (exists(path)) {
      return path;
    }
  }

  umbra::umbra_fail("CLI: failed to find runner executable");
}

int main(const int argc, const char** argv) try {
  const std::filesystem::path project_dir = argc >= 2 ? std::filesystem::path(argv[1]) : std::filesystem::current_path();

  const umbra::Config config = umbra::load_config(project_dir);

  const std::string safe_name = sanitize_alphanumeric(config.name);
  const std::filesystem::path out_dir = std::filesystem::current_path() / safe_name;
  ensure_dir(out_dir);

  std::vector<uint8_t> secret = generate_secret();

  {
    const auto source_writer = std::make_unique<umbra::PakWriter>(out_dir / "src.pak", secret, config.source_dir);
    source_writer->add_tree(config.source_dir);

    const auto assets_writer = std::make_unique<umbra::PakWriter>(out_dir / "ass.pak", secret, config.assets_dir);
    assets_writer->add_tree(config.assets_dir);

    const auto config_writer = std::make_unique<umbra::PakWriter>(out_dir / "cfg.pak", secret, project_dir);
    config_writer->add_file(config.config_file);
  }

  const std::filesystem::path cli_dir = executable_parent_path();

#ifdef _WIN32
  const std::filesystem::path runner_destination = out_dir / (safe_name + ".exe");
#else
  const std::filesystem::path runner_destination = out_dir / safe_name;
#endif

  { // Copy runner to output
    const std::filesystem::path runner_source = find_runner(cli_dir);
    std::error_code ec;
    copy_file(runner_source, runner_destination, std::filesystem::copy_options::overwrite_existing, ec);

    if (ec) {
      umbra::umbra_fail("CLI: failed to copy runner executable ("s + ec.message() + ")");
    }

    append_secret_trailer(runner_destination, secret);
  }

  copy_shared_libs(cli_dir, out_dir);

  std::cout << "Successfully packaged " << absolute(project_dir) << " -> " << absolute(runner_destination) << '\n';

  return 0;
} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
  return 1;
}