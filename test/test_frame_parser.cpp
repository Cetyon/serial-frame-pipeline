#include "serial_frame_pipeline/checksum.hpp"
#include "serial_frame_pipeline/frame_parser.hpp"
#include <cstdint>
#include <array>
#include <iostream>
#include <vector>
#include <span>
#include <algorithm>
#include <optional>

namespace {
  std::string getStateName(sfp::FrameResult state){
    switch (state)
    {
      case sfp::FrameResult::Ready:
          return "Ready";
      case sfp::FrameResult::Incomplete:
          return "Incomplete";
      case sfp::FrameResult::BadChecksum:
          return "BadChecksum";
      case sfp::FrameResult::BadLen:
          return "BadLen";
      case sfp::FrameResult::Heartbeat:
          return "Heartbeat";
      default:
          return "Unknown";
    }
  }
  std::vector<std::uint8_t> makeFrame(std::span<std::uint8_t> payload){
    std::uint8_t len = static_cast<std::uint8_t>(payload.size());
    std::vector<std::uint8_t> output;
    output.reserve(3 + payload.size());
    output.push_back(sfp::kFrameHeader);
    output.push_back(len);
    output.insert(output.end(), payload.begin(), payload.end());
    std::uint8_t checksum = sfp::computeChecksum(std::span{output}.subspan(2));
    output.push_back(checksum);
    return output;
  }
  std::vector<sfp::FrameResult> feedBytes(sfp::FrameParser& parser, std::span<std::uint8_t> frame){
    std::vector<sfp::FrameResult> output;
    std::size_t n = frame.size();
    output.reserve(n);
    for(std::size_t i = 0; i < n; i++){
      sfp::FrameResult result = parser.feed(frame[i]);
      output.push_back(result);
    }
    return output;
  }
  bool checkReturn(std::vector<sfp::FrameResult> input, sfp::FrameResult finalResult){
    std::size_t n = input.size();
    for(std::size_t i = 0; i < n-1; i++){
      switch (input[i]){
        case sfp::FrameResult::Ready:
          std::cout<<"Early jump to Ready state at byte :"<<i<<"\n";
          return false;
        case sfp::FrameResult::Incomplete:
          if (i != n-1) break;
          std::cout<<"Still in state Incomplete after feeding the last byte\n"; 
          return false;
        case sfp::FrameResult::BadChecksum:
          std::cout<<"BadChecksum state after feeding byte "<<i<<"\n"; 
          return false;
        case sfp::FrameResult::BadLen:
          std::cout<<"BadLen state after feeding byte "<<i<<"\n"; 
          return false;
        case sfp::FrameResult::Heartbeat:
          std::cout<<"Heartbeat state after feeding byte "<<i<<"\n"; 
          return false;
        default:
          std::cout<<"Unknown state after feeding byte "<<i<<"\n";
          return false;
      }
    }
    if ((input[n-1]!=finalResult)){
      std::cout<<"Last byte obtained "<<getStateName(input[n-1])<<" instead of " << getStateName(finalResult)<<"\n";
      return false;
    }
    return true;
  }
  
}

bool testValidFrame(){
    sfp::FrameParser parser;
    std::array<std::uint8_t, 2> test1{0x10,0x20};
    std::vector<std::uint8_t> frame = makeFrame(test1);
    std::vector<sfp::FrameResult> res = feedBytes(parser,std::span{frame});
    if (res.size() != frame.size()){
      std::cout<<"Less FrameResult obtained than bytes fed\n";
      return false;
    }
    if (!checkReturn(res,sfp::FrameResult::Ready)) return false;
    std::optional<sfp::Frame> receivedFrame = parser.takeFrame();
    if (receivedFrame == std::nullopt){
      std::cout<<"Received frame is empty\n";
      return false;
    }else if (receivedFrame->len != test1.size()){
      std::cout<<"Length mismatch receivedFrame->len: "<<static_cast<int>(receivedFrame->len)<<" and actual frame len: "<<static_cast<int>(test1.size())<<"\n";
      return false;
    }else if (!std::ranges::equal(test1,receivedFrame->getPayload())){
      std::cout<<"Payload mismatch\n";
      return false;
    }
    return true;
}

bool testBadChecksum(){
    sfp::FrameParser parser;
    std::array<std::uint8_t, 2> test1{0x10,0x20};
    std::vector<std::uint8_t> frame = makeFrame(test1);
    frame[frame.size()-1]++;
    std::vector<sfp::FrameResult> res = feedBytes(parser,std::span{frame});
    if (res.size() != frame.size()){
      std::cout<<"Less FrameResult obtained than bytes fed\n";
      return false;
    }
    if(!checkReturn(res,sfp::FrameResult::BadChecksum)) return false;
    std::optional<sfp::Frame> receivedFrame = parser.takeFrame();
    if (receivedFrame != std::nullopt){
      std::cout<<"Received frame isn't empty\n";
      return false;
    }
    return true;
}
bool testBadLen(){
    sfp::FrameParser parser;
    std::array<std::uint8_t, 2> test1{0x10,0x20};
    std::vector<std::uint8_t> frame = makeFrame(test1);
    frame[1]=sfp::kMaxLen + 1;
    std::vector<sfp::FrameResult> res = feedBytes(parser,std::span{frame});
    if (res.size() != frame.size()){
      std::cout<<"Less FrameResult obtained than bytes fed\n";
      return false;
    }
    if(res[1] != sfp::FrameResult::BadLen){
      std::cout<<"Did not detected bad len\n"; 
      return false;
    }
    std::optional<sfp::Frame> receivedFrame = parser.takeFrame();
    if (receivedFrame != std::nullopt){
      std::cout<<"Received frame isn't empty\n";
      return false;
    }
    frame = makeFrame(test1);
    res = feedBytes(parser,std::span{frame});
    if (res.size() != frame.size()){
      std::cout<<"Parser did not recover after badlen: Less FrameResult obtained than bytes fed\n";
      return false;
    }
    if (!checkReturn(res,sfp::FrameResult::Ready)) return false;
    receivedFrame = parser.takeFrame();
    if (receivedFrame == std::nullopt){
      std::cout<<"Parser did not recover after badlen: Received frame is empty\n";
      return false;
    }else if (receivedFrame->len != test1.size()){
      std::cout<<"Parser did not recover after badlen: Length mismatch receivedFrame->len: "<<static_cast<int>(receivedFrame->len)<<" and actual frame len: "<<static_cast<int>(test1.size())<<"\n";
      return false;
    }else if (!std::ranges::equal(test1,receivedFrame->getPayload())){
      std::cout<<"Parser did not recover after badlen: Payload mismatch\n";
      return false;
    }
    return true;
}

int main(){
    
    if (!testValidFrame()){
      std::cout<<"ValidFrame failed\n";
      return 1;
    }
    if (!testBadChecksum()){
      std::cout<<"Badchecksum test failed\n";
      return 1;
    }
    if(!testBadLen()){
      std::cout<<"BadLen test failed\n";
      return 1;
    } 
    return 0;
}