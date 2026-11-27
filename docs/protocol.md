# Communication protocol

## Frame format

> `0xAA | LEN | payload | CHK`

- `kMaxLen`= 32
- `LEN=0` is considered a heartbeat --> `CHK == 0`
- `CHK` is the XOR of the LEN and payload

## State machine

| State | Input / condition | Action | Next state | Return |
|---|---|---|---| --- |
| WaitHeader | b == 0xAA | — | ReadLen | Incomplete |
| WaitHeader | b != 0xAA | — | WaitHeader | Incomplete |
| ReadLen | b > kMaxLen | — | WaitHeader | BadLen |
| ReadLen | 0 < b <= kMaxLen | chk = b / idx = 0 / len = b | ReadPayload | Incomplete |
| ReadLen | b == 0 | — | CheckHeartbeat | Incomplete |
| CheckHeartbeat | b == 0 | — |  WaitHeader | Heartbeat |
| CheckHeartbeat | b != 0 | — |  WaitHeader | BadChecksum |
| ReadPayload | Any byte | chk ^= b / data[idx++] = b | (idx == len) ? ReadChecksum : ReadPayload | Incomplete |
| ReadChecksum | b == chk | store in lastFrame | WaitHeader | Ready |
| ReadChecksum | b != chk | discard frame | WaitHeader | BadChecksum |


## Known limitations 

### A false start byte can cause valid frames to be lost
For example: `AA 05 | AA 02 10 20 CHK`  
- Noise: `AA 05`
- Actual frame: `AA 02 10 20 CHK`

The parser would think that the actual frame is:  
- Start byte: `AA`
- Payload length: `05`
- Data: `AA 02 10 20 CHK`
- Checksum: First byte of the following frame (that frame would be lost too).

There are multiple approaches to solve this issue:

- Once checksum fails look for other `AA` after the `start byte`. To do this we need to keep received bytes. A circular buffer (stage 2) can store them. 
- Byte stuffing: Encoding the data payload so the frame delimiter byte `0xAA` never appears inside it—either by replacing occurrences with an escape sequence (like SLIP) or by re-encoding the entire block (like COBS).
- Max time between bytes: After a certain wait the current frame is discarded.
- Small `MAX_LEN` to reduce the amount of bytes discarded due to a false `start byte`.

Planned:
- Rescanning after a checksum failure (stage 2).

### XOR Limitations

- Swapping two bytes produces the exact same XOR result `0x12 ^ 0x34 equals 0x34 ^ 0x12`, making it blind to swapped bytes.
- Even number of identical bit flips: If the same bit position flips in two different bytes (e.g., bit 3 flips from 0 to 1 in both Byte 1 and Byte 2), the changes cancel out and the checksum remains valid.

