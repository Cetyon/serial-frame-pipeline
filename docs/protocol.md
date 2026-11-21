# Communication protocol

## Frame format

> `0xAA | LEN | payload | CHK`

- `MAX_LEN`= 64
- `LEN=0` is considered a heartbeat --> `CHK == 0`
- `CHK` is the XOR of the LEN and payload

## State machine

| State | Input / condition | Action | Next state |
|---|---|---|---|
| R_HDR | b == 0xAA | — | R_LEN |
| R_HDR | b != 0xAA | — | R_HDR |
| R_LEN | b > MAX_LEN | — | R_HDR |
| R_LEN | 0 < b <= MAX_LEN | chk = b / idx = 0 / len = b | R_PAYLOAD |
| R_LEN | b == 0 | chk = b / idx = 0 / len = b | R_CHK |
| R_PAYLOAD | Any byte | chk ^= b / data[idx++] = b | (idx == len) ? R_CHK : R_PAYLOAD |
| R_CHK | b == chk | emit Frame{len, data} | R_HDR |
| R_CHK | b != chk | discard frame | R_HDR |


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

