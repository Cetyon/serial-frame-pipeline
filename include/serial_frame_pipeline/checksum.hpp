#pragma once

#include <cstdint>
#include <span>


namespace sfp {
    [[nodiscard]] std::uint8_t computeChecksum(std::span<const std::uint8_t> payload);
}