# Virtual Memory Management Simulator

C++17 coursework simulator for address decomposition, TLB caching, page replacement and simplified memory-latency accounting. It compares FIFO, LRU and OPT on the same hexadecimal address trace, with an optional raylib view.

## Build and run

Requirements: C++17 compiler, CMake 3.16+, and JsonCpp. On Ubuntu/Debian:

```sh
sudo apt install build-essential cmake libjsoncpp-dev
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
mkdir -p run
cp Parameters.json addresses.txt run/
cd run
../build/memory_simulator
```

The program reads input files from the working directory and overwrites its output logs. Use `run/` to preserve committed examples. On Windows install JsonCpp through vcpkg and configure CMake with its toolchain; for Visual Studio builds the executable is under `build/Debug/` (or the selected configuration).

The console executable runs FIFO, LRU and OPT sequentially. The optional GUI needs raylib 5.5:

```sh
cmake -S . -B build -DBUILD_GUI=ON
cmake --build build
# From run/ after copying the inputs:
../build/memory_gui fifo
# Alternatives: lru, opt. Close the window to end a run.
```

## Architecture and inputs

| Source | Responsibility |
|---|---|
| `FileConfiguration.*` | JSON parameters, hexadecimal trace, VPN/offset calculation |
| `TLB_table.*` | FIFO/LRU/OPT cache policies and hit/miss metrics |
| `Page_frame.*` | Page table, frame assignment, eviction, dirty tracking |
| `Ram_memory.*` | In-memory byte arrays |
| `Virtual_Memory_manager.*` | Console execution paths |
| `Drawer.cpp`, `mem_visualization.h` | Optional GUI |
| `OS_project.cpp`, `gui_main.cpp` | Console and GUI entry points |

`Parameters.json` currently specifies 262144 bytes of RAM, 4096-byte pages, 16 TLB entries and latency 10. `virtual_size` is treated by this implementation as an **inclusive maximum address**, not a byte count. Page size must be a positive power of two, RAM must hold whole pages and TLB capacity must be positive.

`addresses.txt` contains whitespace-separated hexadecimal addresses. Every fifth access, starting at index 0, is treated as a write; the remaining accesses are reads. The input does not encode arbitrary read/write operations.

## Outputs and interpretation

- `comparison_metrics.csv`: FIFO/LRU/OPT request counts, TLB hits/misses, hit rate, RAM counters, dirty evictions and EAT.
- `FIFO_logging.txt`, `log.txt`, `OPT_logging`: policy traces.
- `HardDisk.txt`: appended dirty-frame dumps; reset between policy runs.

EAT is a simulated weighted cost, not measured CPU performance: lookup 1 ns/request, TLB miss 10 ns, RAM read `latency`, write `2*latency`, dirty eviction 1000 ns. Final destructor flushing is not included in the printed eviction/EAT metrics. RAM-read accounting differs between code paths, so EAT comparisons need care.

## Verification and limitations

Core checks cover known VPN/offset decomposition, LRU cache eviction, invalidation and FIFO dirty-frame replacement. CI builds the console and optional GUI and runs the checks.

This is not a full persistent virtual memory implementation: evicted page contents are dumped but not read back by VPN, and frame reuse does not restore backing-store contents. `HardDisk.txt` is therefore an educational write-back log, not a true swap file. OPT uses future trace knowledge as an offline comparison policy. GUI entries iterate an unordered map and do not represent fixed hardware slots.

The original report and screenshots are retained below as historical coursework material. Their settings may differ from the included default configuration. A license and per-person team contributions require the owner's decision.

**GUI OUTPUT**
For this data
<img width="1291" height="664" alt="image" src="https://github.com/user-attachments/assets/825d35cc-e2c1-48e0-8436-d0a727908f23" />
                                                      **using 4 Page Frames**

For FIFO:
  <img width="1127" height="781" alt="image" src="https://github.com/user-attachments/assets/b5ab213b-6d02-4ed5-acac-02e52d52217c" />
For LRU:
   <img width="1105" height="780" alt="image" src="https://github.com/user-attachments/assets/8fc4559b-e34f-4c39-8b35-85af22b23fe1" />
For OPT:
   <img width="1113" height="766" alt="image" src="https://github.com/user-attachments/assets/92c2eca2-4a25-4c12-892d-6858a68b5eba" />
