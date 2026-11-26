#include "serial_frame_pipeline/checksum.hpp"
#include <cstdint>
#include <array>
#include <iostream>


int main(){
    std::array<std::uint8_t, 2> test2{0x10,0x20};
    std::uint8_t chk1 = sfp::computeChecksum({});
    if (chk1 != 0) {
        std::cout << "checksum {}: expected 0x" << std::hex << static_cast<int>(0x00)
          << ", got 0x" << static_cast<int>(chk1) << std::dec << '\n';
        return 1;
    }
    std::uint8_t chk2 = sfp::computeChecksum(test2);
    if (chk2 != static_cast<std::uint8_t>(0x32)){ 
        std::cout << "checksum {0x10, 0x20}: expected 0x" << std::hex << static_cast<int>(0x32)
          << ", got 0x" << static_cast<int>(chk2) << std::dec << '\n';
        return 1;
    }
    return 0;
}