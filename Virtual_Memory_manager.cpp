#include "Virtual_Memory_manager.h"

void Virtual_Memory_manager::Runner() {
  ofstream clear_disk("HardDisk.txt", ios::trunc);
  ofstream clear_log("log.txt", ios::trunc);
  clear_log.close();
  FileConfiguration config;
  config.FileParser();
  config.Display();
  unsigned long long frame_address;
  TLB cache(config);
  Page_table pages(config);

  config.Reading_Addresses();
  int instruction_count = 0;
  size_t index = 0;
  for (auto it = config.actual_adress.begin(); it != config.actual_adress.end();
       it++) {
    unsigned long long address = *it;
    unsigned long long vpn = config.getVirtual_page_number(address);
    unsigned long long offset = config.offset(address);

    bool is_write = (instruction_count % 5 == 0);
    char x;
    if (is_write) {
      x = 'W';
    } else {
      x = 'R';
    }

    ofstream lru_log("log.txt", ios::app);

    bool found = cache.find(vpn, frame_address);
    if (found) {
      if (lru_log.is_open()) {
        lru_log << "[LRU] Request #" << instruction_count
                << " | Addr: 0x" << hex << address << dec
                << " | VPN: " << vpn
                << " | Op: " << x
                << " | TLB HIT  -> Frame: " << frame_address << endl;
      }
      pages.getPhysicalAddress(vpn, offset, is_write, cache, found, x);
    }
    if (!found) {
      unsigned long long p_frame = pages.getPhysicalAddress(vpn, offset, is_write, cache, x);
      if (lru_log.is_open()) {
        lru_log << "[LRU] Request #" << instruction_count
                << " | Addr: 0x" << hex << address << dec
                << " | VPN: " << vpn
                << " | Op: " << x
                << " | TLB MISS -> Frame: " << p_frame << endl;
      }
      cache.insert_with_LRU_algo(vpn, p_frame, true);
    }

    lru_log.close();
    index++;
    instruction_count++;
  }

  cout << "The frames used are " << pages.getFrame_count();
  cache.perfomance_metrics(config.latency, pages.ram_read_count,
                           pages.ram_write_count, pages.dirty_evictions, "LRU");
}
void Virtual_Memory_manager::SPRINTER() {

  ofstream clear_disk("HardDisk.txt", ios::trunc);
  ofstream clear_disk2("OPT_logging", ios::trunc);
  FileConfiguration config;
  config.FileParser();
  config.Display();
  unsigned long long frame_address;
  TLB cache(config);
  Page_table pages(config);

  config.Reading_Addresses();
  pages.LOOKAHEADINSERTION(config);

  int instruction_count = 0;
  size_t index = 0;
  for (auto it = config.actual_adress.begin(); it != config.actual_adress.end();
       it++) {
    unsigned long long address = *it;
    unsigned long long vpn = config.getVirtual_page_number(address);
    unsigned long long offset = config.offset(address);

    bool is_write = (instruction_count % 5 == 0);
    char x;
    if (is_write) {
      x = 'W';
    } else {
      x = 'R';
    }

    bool found = cache.find(vpn, frame_address);
    if (found) {
      pages.accessPageTableDirectly(vpn, offset, is_write, x);
    }
    if (!found) {

      unsigned long long p_frame =
          pages.accessPageTableDirectly(vpn, offset, is_write, x);
      if (p_frame == (unsigned long long)-1) {
        p_frame = pages.OPT_algorthim_for_getting_pshycal_number(
            vpn, offset, is_write, cache, config.actual_adress, index, x);
      }
      cache.insert_with_LRU_algo(vpn, p_frame, true);
    }

    index++;
    instruction_count++;
  }

  cout << "The frames used are " << pages.getFrame_count();
  cache.perfomance_metrics(config.latency, pages.ram_read_count,
                           pages.ram_write_count, pages.dirty_evictions, "OPT_RAM");
}

void Virtual_Memory_manager::Walker() {

  ofstream clear_disk("HardDisk.txt", ios::trunc);
  ofstream clear_disk2("OPT_logging", ios::trunc);
  ofstream clear_fifo_log("FIFO_logging.txt", ios::trunc);
  clear_fifo_log.close();
  FileConfiguration config;
  config.FileParser();
  config.Display();
  unsigned long long frame_address;
  TLB cache(config);
  Page_table pages(config);

  config.Reading_Addresses();

  int instruction_count = 0;
  size_t index = 0;
  for (auto it = config.actual_adress.begin(); it != config.actual_adress.end();
       it++) {
    unsigned long long address = *it;
    unsigned long long vpn = config.getVirtual_page_number(address);
    unsigned long long offset = config.offset(address);

    bool is_write = (instruction_count % 5 == 0);
    char x;
    if (is_write) {
      x = 'W';
    } else {
      x = 'R';
    }

    ofstream fifo_log("FIFO_logging.txt", ios::app);

    bool found = cache.findFIFOTLB(vpn, frame_address); // this is the TLB
                                                        // lookup
    if (found) {
      if (fifo_log.is_open()) {
        fifo_log << "[FIFO] Request #" << instruction_count
                 << " | Addr: 0x" << hex << address << dec
                 << " | VPN: " << vpn
                 << " | Op: " << x
                 << " | TLB HIT  -> Frame: " << frame_address << endl;
      }
      pages.accessPageTableDirectly(vpn, offset, is_write, x);
    }
    if (!found) {
      unsigned long long p_frame =
          pages.accessPageTableDirectly(vpn, offset, is_write, x);
      if (p_frame == (unsigned long long)-1) {
        if (fifo_log.is_open()) {
          fifo_log << "[FIFO] Request #" << instruction_count
                   << " | Addr: 0x" << hex << address << dec
                   << " | VPN: " << vpn
                   << " | Op: " << x
                   << " | TLB MISS -> PAGE FAULT (loading from disk)" << endl;
        }
        p_frame = pages.acccess_using_FIFO(vpn, offset, is_write, cache, x);
      } else {
        if (fifo_log.is_open()) {
          fifo_log << "[FIFO] Request #" << instruction_count
                   << " | Addr: 0x" << hex << address << dec
                   << " | VPN: " << vpn
                   << " | Op: " << x
                   << " | TLB MISS -> Page Table HIT -> Frame: " << p_frame << endl;
        }
      }
      cache.Insert_with_FIFO_algo(vpn, p_frame, true);
    }

    fifo_log.close();
    index++;
    instruction_count++;
  }

  cout << "The frames used are " << pages.getFrame_count();
  cache.perfomance_metrics(config.latency, pages.ram_read_count,
                           pages.ram_write_count, pages.dirty_evictions, "FIFO");
}

void Virtual_Memory_manager::SPRINTER_OPT_TLB() {

  ofstream clear_disk("HardDisk.txt", ios::trunc);
  ofstream clear_disk2("OPT_logging", ios::trunc);
  clear_disk2.close();
  FileConfiguration config;
  config.FileParser();
  config.Display();
  unsigned long long frame_address;
  TLB cache(config);
  Page_table pages(config);

  config.Reading_Addresses();

  cache.OPT_LOOKAHEAD_INSERTION(config);
  pages.LOOKAHEADINSERTION(config);

  int instruction_count = 0;
  size_t index = 0;
  for (auto it = config.actual_adress.begin(); it != config.actual_adress.end();
       it++) {
    unsigned long long address = *it;
    unsigned long long vpn = config.getVirtual_page_number(address);
    unsigned long long offset = config.offset(address);

    bool is_write = (instruction_count % 5 == 0);
    char x;
    if (is_write) {
      x = 'W';
    } else {
      x = 'R';
    }

    ofstream opt_log("OPT_logging", ios::app);

    bool found = cache.findOPT_TLB(vpn, frame_address);
    if (found) {
      if (opt_log.is_open()) {
        opt_log << "[OPT] Request #" << instruction_count
                << " | Addr: 0x" << hex << address << dec
                << " | VPN: " << vpn
                << " | Op: " << x
                << " | TLB HIT  -> Frame: " << frame_address << endl;
      }
      pages.accessPageTableDirectly(vpn, offset, is_write, x);
    }
    if (!found) {
      unsigned long long p_frame =
          pages.accessPageTableDirectly(vpn, offset, is_write, x);
      if (p_frame == (unsigned long long)-1) {
        if (opt_log.is_open()) {
          opt_log << "[OPT] Request #" << instruction_count
                  << " | Addr: 0x" << hex << address << dec
                  << " | VPN: " << vpn
                  << " | Op: " << x
                  << " | TLB MISS -> PAGE FAULT (loading from disk)" << endl;
        }
        p_frame = pages.OPT_algorthim_for_getting_pshycal_number(
            vpn, offset, is_write, cache, config.actual_adress, index, x);
      } else {
        if (opt_log.is_open()) {
          opt_log << "[OPT] Request #" << instruction_count
                  << " | Addr: 0x" << hex << address << dec
                  << " | VPN: " << vpn
                  << " | Op: " << x
                  << " | TLB MISS -> Page Table HIT -> Frame: " << p_frame << endl;
        }
      }

      cache.insert_with_OPT_algo(vpn, p_frame, true, index);
    }

    opt_log.close();
    index++;
    instruction_count++;
  }

  cout << "The frames used are " << pages.getFrame_count();
  cache.perfomance_metrics(config.latency, pages.ram_read_count,
                           pages.ram_write_count, pages.dirty_evictions, "OPT");
}
