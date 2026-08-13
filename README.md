
# Virtual Memory Management Simulator

## About

A high-performance C++ simulator that models the complete MMU pipeline, including TLB caching, page table translation, and hardware latency metrics. It implements FIFO, LRU, and Optimal replacement algorithms to analyze page fault rates and calculate the system's Effective Access Time (EAT).

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Page Replacement Algorithms](#page-replacement-algorithms)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [Input Format](#input-format)
- [Performance Metrics](#performance-metrics)
- [How It Works](#how-it-works)

---

## Overview

This project simulates the **complete virtual-to-physical memory translation pipeline** found in modern operating systems. It models the following hardware and software components:

| Component | Function |
| :--- | :--- |
| **TLB (Translation Lookaside Buffer)** | A small, fast cache for recent VPN → Frame translations. |
| **Page Table** | The full VPN → Physical Frame mapping with dirty/valid bits. |
| **RAM** | A byte-addressable physical memory array simulation. |
| **Hard Disk** | The backing store for dirty page write-backs and page loading. |

The simulator processes a stream of hexadecimal virtual addresses, translates them through the hierarchy, applies replacement policies during memory pressure, and reports final performance analytics.

---

## Architecture



```text
┌─────────────────────────────────────────────────────────┐
│                     Virtual Address                     │
│              ┌──────────────┬──────────┐                │
│              │     VPN      │  Offset  │                │
│              └──────┬───────┴─────┬────┘                │
│                     │             │                     │
│              ┌──────▼──────┐      │                     │
│              │     TLB     │      │                     │
│              | (has algos) │      │                     │
│              └──┬─────┬─── ┘      │                     │
│            Hit  │     │ Miss      │                     │
│              ┌──▼─────▼───┐       │                     │
│              │ Page Table │       │                     │
│              │ (FIFO/LRU/ │       │                     │
│              │    OPT)    │       │                     │
│              └──────┬──────┘      │                     │
│                     │ Frame #     │                     │
│              ┌──────▼─────────────▼───┐                 │
│              │         RAM            │                 │
│              │  memory[frame][offset] │                 │
│              └──────────┬─────────────┘                 │
│                         │ Dirty eviction                │
│              ┌──────────▼─────────────┐                 │
│              │     HardDisk.txt       │                 │
│              │    (Backing Store)     │                 │
│              └────────────────────────┘                 │
└─────────────────────────────────────────────────────────┘
```

---

## Page Replacement Algorithms

The simulator implements **three** classic strategies, executed independently on the same address trace for performance comparison:

### 1. FIFO (First-In, First-Out)
* **Logic**: Evicts the **oldest** page in memory—the one loaded first.
* **Implementation**: Simple queue-based logic using `std::list`.
* **Logging**: Traces every eviction event to `FIFO_logging.txt`.

### 2. LRU (Least Recently Used)
* **Logic**: Evicts the page that hasn't been accessed for the **longest time**.
* **Implementation**: $O(1)$ lookup and update via `unordered_map` paired with a doubly-linked list.
* **Logging**: Traces eviction events to `log.txt`.

### 3. OPT (Optimal / Bélády's Algorithm)
* **Logic**: Evicts the page that will **not be used for the longest time** in the future.
* **Implementation**: Pre-computes a lookahead buffer of all future VPN references to determine the ideal victim.
* **Logging**: Traces results to `OPT_logging`.

---

## Project Structure

* **`OS_project.cpp`**: Main entry point; executes FIFO, LRU, and OPT sequentially.
* **`Virtual_Memory_manager.h/cpp`**: Orchestration logic for the three simulation modes.
* **`TLB_table.h/cpp`**: TLB cache implementation, management, and metrics.
* **`Page_frame.h/cpp`**: Page table logic, dirty-bit tracking, and disk I/O.
* **`Ram_memory.h/cpp`**: Physical memory array and `Page_frame_node` definitions.
* **`FileConfiguration.h/cpp`**: Helper class to parse JSON parameters and hexadecimal addresses.
* **`Parameters.json`**: Global simulation settings (memory size, latencies).
* **`addresses.txt`**: Input file containing hexadecimal virtual addresses.

---

## Configuration

Modify **`Parameters.json`** to stress-test the system:

| Parameter | Description | Default |
| :--- | :--- | :--- |
| `physical_size` | Total physical memory (Bytes) | `40960` (40 KB) |
| `page_size` | Size of each page (Bytes) | `4096` (4 KB) |
| `TLB_size` | Number of TLB cache entries | `4` |
| `latency` | RAM access latency (ns) | `10` |

---

## Performance Metrics

The simulator calculates several key indicators to determine efficiency:

* **TLB Hit Rate**: Percentage of addresses resolved by the cache.
* **RAM Reads/Writes**: Total data movement between the CPU and simulated RAM.
* **Dirty Evictions**: Number of times a modified page had to be written back to disk.
* **Effective Access Time (EAT)**: The weighted average time per request.

### EAT Calculation
$$EAT = \frac{(Req \times 1ns) + (Miss \times 10ns) + (Reads \times Lat) + (Writes \times Lat \times 2) + (Dirty \times 1000ns)}{Total Requests}$$

---

## How It Works

1.  **Address Decomposition**: Virtual addresses are split into **VPN** and **Offset** via bitwise shifting.
2.  **Hierarchical Lookup**:
    * **TLB Hit**: Instant frame resolution.
    * **TLB Miss**: Walk the Page Table.
    * **Page Fault**: Load from `HardDisk.txt`, potentially triggering eviction.
3.  **Eviction Cycle**: If RAM is full, the chosen algorithm selects a victim. If the **dirty bit** is set, it triggers a 1000ns write-back penalty to `HardDisk.txt`.
4.  **Shutdown**: Upon termination, all remaining dirty pages are flushed to disk to ensure data persistence.


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



