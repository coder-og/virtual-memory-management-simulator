#include "TLB_table.h"
#include <algorithm>

Value::Value() { valid_bit = true; }

TLB::TLB(FileConfiguration &config) {
  cap = config.TLB_size;
  tlb_misses = 0;
  tlb_hits = 0;
  request = 0;
  count = 0;
}

void TLB::View_table() {

  for (unordered_map<unsigned long long, Value>::iterator it = table.begin();
       it != table.end(); it++) {
    cout << "Virtual Address: " << it->first
         << ", Physical Memory: " << it->second.physical_memory
         << ", Valid Bit: " << it->second.valid_bit << endl;
  }

  cout << "\t--------------Table Displayed----------------" << endl;
}

void TLB::Insert_with_FIFO_algo(unsigned long long virtual_address_key,
                                unsigned long long physical_memory_value,
                                bool valid_bit_value) {
  Value value;
  value.physical_memory = physical_memory_value;
  value.valid_bit = valid_bit_value;
  if (table.find(virtual_address_key) != table.end()) {
    return;
  }
  if (table.size() < cap) {
    table[virtual_address_key] = value;
    index_for_FIFO.push_back(virtual_address_key);
    count++;
  } else {
    unsigned long long vpn_to_evict = index_for_FIFO.front();
    int index = getVPNIndex(vpn_to_evict);
    if (index != -1) {
      lastEvictedSlot = index;
    }
    table.erase(index_for_FIFO.front());
    index_for_FIFO.pop_front();
    table[virtual_address_key] = value;
    index_for_FIFO.push_back(virtual_address_key);
    count++;
  }
}

void TLB::insert_with_LRU_algo(unsigned long long virtual_address_key,
                               unsigned long long physical_memory_frame,
                               bool valid_bit_value) {
  Value v;
  v.physical_memory = physical_memory_frame;
  v.valid_bit = valid_bit_value;
  if (table.find(virtual_address_key) != table.end()) {
    table[virtual_address_key] = v;
    LRU_queue.erase(LRU_map[virtual_address_key]);
    LRU_queue.push_front(virtual_address_key);
    LRU_map[virtual_address_key] = LRU_queue.begin();
    return;
  }
  if (table.size() < cap) {
    table[virtual_address_key] = v;
    LRU_queue.push_front(virtual_address_key);
    LRU_map[virtual_address_key] = LRU_queue.begin();
    count++;
    return;
  } else {
    unsigned long long least_used = LRU_queue.back();
    int index = getVPNIndex(least_used);
    if (index != -1) {
      lastEvictedSlot = index;
    }
    LRU_queue.pop_back();
    if (LRU_map.find(least_used) != LRU_map.end()) {
      LRU_map.erase(least_used);
    }
    if (table.find(least_used) != table.end()) {
      table.erase(least_used);
    }
    table[virtual_address_key] = v;
    LRU_queue.push_front(virtual_address_key);
    LRU_map[virtual_address_key] = LRU_queue.begin();
    return;
  }
}

bool TLB::find(unsigned long long virtual_address_key,
               unsigned long long &physical_memory_value) {
  request++;
  if (table.find(virtual_address_key) != table.end()) {

    if (table.at(virtual_address_key).valid_bit) {
      physical_memory_value = table.at(virtual_address_key).physical_memory;
      tlb_hits++;
      LRU_queue.erase(LRU_map.at(virtual_address_key));
      LRU_queue.push_front(virtual_address_key);
      LRU_map[virtual_address_key] = LRU_queue.begin();

      return true;
    } else {
      tlb_misses++;
      return false;
    }

  } else {
    tlb_misses++;
    return false;
  }
}
bool TLB::findFIFOTLB(unsigned long long virtual_address_key,
                      unsigned long long &physical_memory_value) {
  request++;
  if (table.find(virtual_address_key) != table.end()) {

    if (table.at(virtual_address_key).valid_bit) {
      physical_memory_value = table.at(virtual_address_key).physical_memory;
      tlb_hits++;

      return true;
    } else {
      tlb_misses++;
      return false;
    }

  } else {
    tlb_misses++;
    return false;
  }
}

void TLB::perfomance_metrics(unsigned long long ram_latency,
                             unsigned long long total_ram_reads,
                             unsigned long long total_ram_writes,
                             unsigned long long dirty_evictions,
                             string algo_name) {
  float hit_rate = 0;
  if (request > 0) {
    hit_rate = (float)tlb_hits / (float)request;
  }

  unsigned long long tlb_misses = request - tlb_hits;

  double total_time = 0;
  total_time += (double)request * 1.0;     // TLB lookup: 1 ns
  total_time += (double)tlb_misses * 10.0; // TLB miss penalty: 10 ns
  total_time += (double)total_ram_reads * ram_latency; // RAM reads
  total_time +=
      (double)total_ram_writes * (ram_latency * 2); // RAM writes: 2x penalty
  total_time += (double)dirty_evictions * 1000.0;   // Disk write-back: 1000 ns

  double eat = 0;
  if (request > 0) {
    eat = total_time / request;
  }

  cout << "\n--- Simulation Results (" << algo_name << ") ---" << endl;
  cout << "Total Requests: " << request << endl;
  cout << "TLB Hit Rate: " << hit_rate * 100 << "%" << endl;
  cout << "RAM Reads: " << total_ram_reads << endl;
  cout << "RAM Writes: " << total_ram_writes << endl;
  cout << "Dirty Evictions: " << dirty_evictions << endl;
  cout << "Effective Access Time (EAT): " << eat << " ns" << endl;
  cout << "--------------------------" << endl;

  bool write_header = false;
  ifstream check_csv("comparison_metrics.csv");
  if (!check_csv.good() || check_csv.peek() == ifstream::traits_type::eof()) {
    write_header = true;
  }
  check_csv.close();

  ofstream csv("comparison_metrics.csv", ios::app);
  if (csv.is_open()) {
    if (write_header) {
      csv << "Algorithm,TotalRequests,TLBHits,TLBMisses,HitRate,RAMReads,"
             "RAMWrites,DirtyEvictions,EAT"
          << endl;
    }
    csv << algo_name << "," << request << "," << tlb_hits << "," << tlb_misses
        << "," << hit_rate * 100 << "," << total_ram_reads << ","
        << total_ram_writes << "," << dirty_evictions << "," << eat << endl;
    csv.close();
  }
}

int TLB::getFileCount() { return count; }

void TLB::TLB_Eviction(unsigned long long vpn) {
  int index = getVPNIndex(vpn);
  list<unsigned long long>::iterator temp_fifo =
      std::find(index_for_FIFO.begin(), index_for_FIFO.end(), vpn);
  if (temp_fifo != index_for_FIFO.end()) {
    index_for_FIFO.erase(temp_fifo);
  }

  if (table.find(vpn) != table.end()) {
    table.erase(vpn);
    count--;
  }

  if (LRU_map.find(vpn) != LRU_map.end()) {
    LRU_queue.erase(LRU_map.at(vpn));
    LRU_map.erase(vpn);
  }
  if (index != -1) {
    lastEvictedSlot = index;
  }
}

void TLB::OPT_LOOKAHEAD_INSERTION(FileConfiguration &config) {
  OPT_lookahead_buffer.clear();
  for (auto it = config.actual_adress.begin(); it != config.actual_adress.end();
       it++) {
    OPT_lookahead_buffer.push_back(config.getVirtual_page_number(*it));
  }
}

bool TLB::findOPT_TLB(unsigned long long virtual_address_key,
                      unsigned long long &physical_memory_value) {
  request++;
  if (table.find(virtual_address_key) != table.end()) {
    if (table.at(virtual_address_key).valid_bit) {
      physical_memory_value = table.at(virtual_address_key).physical_memory;
      tlb_hits++;
      return true;
    } else {
      tlb_misses++;
      return false;
    }
  } else {
    tlb_misses++;
    return false;
  }
}

void TLB::insert_with_OPT_algo(unsigned long long virtual_address_key,
                               unsigned long long physical_memory_frame,
                               bool valid_bit_value, size_t current_index) {
  Value v;
  v.physical_memory = physical_memory_frame;
  v.valid_bit = valid_bit_value;

  if (table.find(virtual_address_key) != table.end()) {
    table[virtual_address_key] = v;
    return;
  }

  if (table.size() < cap) {
    table[virtual_address_key] = v;
    count++;
    return;
  }

  unsigned long long victim_vpn = table.begin()->first;
  size_t max_distance = 0;
  bool found_never_used = false;

  for (auto const &[vpn_in_tlb, val] : table) {
    bool will_be_used = false;
    size_t distance = 0;

    for (size_t j = current_index + 1; j < OPT_lookahead_buffer.size(); j++) {
      if (OPT_lookahead_buffer[j] == vpn_in_tlb) {
        distance = j - current_index;
        will_be_used = true;
        break;
      }
    }

    if (!will_be_used) {
      victim_vpn = vpn_in_tlb;
      found_never_used = true;
      break;
    } else if (distance > max_distance) {
      max_distance = distance;
      victim_vpn = vpn_in_tlb;
    }
  }

  int idx = getVPNIndex(victim_vpn);
  if (idx != -1) {
    lastEvictedSlot = idx;
  }

  table.erase(victim_vpn);
  table[virtual_address_key] = v;
  count++;
}
