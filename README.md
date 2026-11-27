# Serial Frame Pipeline

![CI](https://github.com/Cetyon/serial-frame-pipeline/actions/workflows/ci.yml/badge.svg)

> Personal learning project featuring a serial binary protocol frame `0xAA | LEN | payload | CHK` pipeline with threads and communication between processes.

**Status:** `under development`  
**Author:** Jorge Rodríguez Rubio

---

## 1. Requirements

| Component | Version |
| --------- | ------- |
| Ubuntu | 24.04 |
| g++ | 13.3 |
| CMake | >=3.28 |
| C++   | >=20 |

## 2. Installation & build

### 2.1 Repository cloning

```bash
git clone https://github.com/Cetyon/serial-frame-pipeline.git
cd serial-frame-pipeline
```
### 2.2 Repository building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
## 3. Tests

Tests currently covers checksum computation, valid frame, wrong checksum and invalid length + parser recovery. 

```bash
ctest --test-dir build --output-on-failure
```

## 4. Usage

```bash
./build/frame_app
```

## 5. Protocol

Frames have the following structure:  
- `0xAA`: start byte.
- `LEN`: payload length.
- `payload`: data.
- `CHK`: XOR of LEN and payload.

See the [protocol specification](docs/protocol.md) for the parser state machine and known limitations.

## 6. Current status & next steps

**Working (Nov 27, 2026)**
- Test program compiles and runs properly.
- Protocol and state machine are documented.
- FrameParser class to read binary protocol frames `0xAA | LEN | payload | CHK` implemented as a state machine with tests.
- CI with GitHub Actions: builds and runs all tests on every push.

**Future steps**
- Add test for heartbeat, noise, two consecutive frames and split frame. 
- Implement a circular buffer.
- Thread-safe queue between two threads (producer/consumer).
- Latency measurement.
- Inter-process communication (IPC) with sockets. 
- Documentation and polish (README with diagram, results). 

