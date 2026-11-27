#pragma once
#include <cstdint>
#include <array>
#include <span>
#include <optional>
#include <cstddef>
#include <utility>
#include "serial_frame_pipeline/checksum.hpp"

namespace sfp{
    constexpr std::uint8_t kFrameHeader = 0xAA;
    constexpr std::size_t kMaxLen = 32;

    struct Frame{
        std::uint8_t len = 0;
        std::array<std::uint8_t, kMaxLen> payload = {};
        std::span<const std::uint8_t> getPayload() const {
            return std::span{payload}.subspan(0,len);
        } 
    };

    enum class FrameResult {
        Ready,
        Incomplete,
        BadChecksum,
        BadLen,
        Heartbeat
    };

    class FrameParser{
        public:
            [[nodiscard]] FrameResult feed(std::uint8_t b);
            [[nodiscard]] std::optional<Frame> takeFrame();
            void reset();
        private:
            enum class State{
                WaitHeader,
                ReadLen,
                ReadPayload,
                ReadChecksum,
                CheckHeartbeat
            };
            State currentState = State::WaitHeader;
            Frame currentFrame;
            std::optional<Frame> lastFrame;
            std::size_t idx = 0;
            std::uint8_t checksum = 0x00;
    };
}