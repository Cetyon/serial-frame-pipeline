# Serial Frame Pipeline

> Personal learning project featuring a serial binary protocol frame (0xAA | LEN | payload | CHK) pipeline with threads and communication between processes.

**Status:** `under development` -- Freshly created repository (Nov 20, 2026).  
**Author:** Jorge Rodríguez Rubio

---

## 1. Requirements

| Component | Version |
| --------- | ------- |
| Ubuntu | 24.04 |
| g++ | 13.3 |
| CMake | >=3.28 |

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

## 3. Usage

```bash
./build/frame_app
```

## 4. Current status & next steps

**Working (Nov 20, 2026)**
- Test program compiles and runs properly

**Future steps**
- Create a FrameParser class to read binary protocol frames (0xAA | LEN | payload | CHK). 
- Implement a circular buffer and CI.
- Thread-safe queue between two threads (producer/consumer).
- Latency measurement.
- Inter-process communication (IPC) with sockets. 
- Documentation and polish (README with diagram, results). 
