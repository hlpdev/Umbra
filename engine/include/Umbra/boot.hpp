#pragma once

#include "umbra.hpp"

#include <cstdint>

namespace umbra {
  constexpr auto SECRET_KEY_MAGIC = "UMBRAKEY\0";
  constexpr size_t SECRET_KEY_MAGIC_LEN = sizeof(SECRET_KEY_MAGIC); // NOLINT(*-sizeof-expression)

#pragma pack(push, 1)
  struct EmbeddedFooter {
    char magic[SECRET_KEY_MAGIC_LEN];
    uint32_t version;
    uint32_t key_length;

    uint64_t reserved;
  };
#pragma pack(pop)

  UMBRA_API EmbeddedFooter make_footer(uint32_t key_length);
}