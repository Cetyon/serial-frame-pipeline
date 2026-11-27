#include "serial_frame_pipeline/frame_parser.hpp"

namespace sfp{
    FrameResult FrameParser::feed(std::uint8_t b){
        switch (currentState)
        {
        case State::WaitHeader:
            if ( b == kFrameHeader) currentState = State::ReadLen;
            break;
        case State::ReadLen:
            if ( b > kMaxLen){
                currentState = State::WaitHeader;
                return FrameResult::BadLen;
            }else if (b == 0){
                currentState = State::CheckHeartbeat;
                break;
            }
            currentFrame.len = b;
            idx = 0;
            checksum = b;
            currentState = State::ReadPayload;
            break;
        case State::ReadPayload:
            currentFrame.payload[idx++] = b;
            checksum ^= b;
            if ( idx == currentFrame.len) currentState = State::ReadChecksum;
            break;
        case State::ReadChecksum:
            if ( b != checksum){
                reset();
                return FrameResult::BadChecksum;
            }
            lastFrame = currentFrame;
            reset();
            return FrameResult::Ready;
        case State::CheckHeartbeat:
            reset();
            if (b == 0) return FrameResult::Heartbeat;
            return FrameResult::BadChecksum;
        }
        return FrameResult::Incomplete;
    }
    std::optional<Frame> FrameParser::takeFrame(){
       return std::exchange(lastFrame, std::nullopt);
    }
    void FrameParser::reset(){
        idx = 0;
        checksum = 0; 
        currentState = State::WaitHeader;
    }
}
