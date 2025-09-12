#include "Umbra/boot.hpp"

#include <cstring>

umbra::EmbeddedFooter umbra::make_footer(const uint32_t key_length) {
  EmbeddedFooter footer{};
  std::memcpy(footer.magic, SECRET_KEY_MAGIC, SECRET_KEY_MAGIC_LEN);
  footer.version = UMBRA_VERSION;
  footer.key_length = key_length;
  footer.reserved = 0;

  return footer;
}
