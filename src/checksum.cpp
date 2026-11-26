#include "serial_frame_pipeline/checksum.hpp"

namespace sfp{
    std::uint8_t computeChecksum(std::span<const std::uint8_t> payload){
        std::uint8_t chk = payload.size();
        for (std::uint8_t x : payload){
            chk ^= x;
        }
        return chk;
    }
}
