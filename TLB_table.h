#pragma once
#include <iostream>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <cmath>
#include <vector>
#include "FileConfiguration.h"

using namespace std;

struct Value {
	unsigned long long physical_memory;
	bool valid_bit;
	Value();
};


struct TLBVisualEntry { unsigned long long vpn; unsigned long long frame; bool active; };



class TLB {
	unsigned long long cap;
	unordered_map<unsigned long long, Value> table;
	list<unsigned long long> index_for_FIFO;
	unordered_map<unsigned long long, typename list<unsigned long long>::iterator> LRU_map;
	list<unsigned long long> LRU_queue;
	vector<unsigned long long> OPT_lookahead_buffer;  // future VPN sequence for OPT
	

public:
	int request;
	int tlb_hits;
	int tlb_misses;
	int count;
	int lastEvictedSlot = -1;
	TLB(FileConfiguration& config);

	void View_table();

	void Insert_with_FIFO_algo(unsigned long long virtual_address_key, unsigned long long physical_memory_value, bool valid_bit_value);

	void insert_with_LRU_algo(unsigned long long virtual_address_key, unsigned long long physical_memory_frame, bool valid_bit_value);


	void OPT_LOOKAHEAD_INSERTION(FileConfiguration& config);  
	void insert_with_OPT_algo(unsigned long long virtual_address_key, unsigned long long physical_memory_frame, bool valid_bit_value, size_t current_index);
	bool findOPT_TLB(unsigned long long virtual_address_key, unsigned long long& physical_memory_value);

	bool find(unsigned long long virtual_address_key, unsigned long long& physical_memory_value);

	void perfomance_metrics(unsigned long long ram_latency, unsigned long long total_ram_reads, unsigned long long total_ram_writes, unsigned long long dirty_evictions, string algo_name = "Unknown");

	int getFileCount();
	bool findFIFOTLB(unsigned long long virtual_address_key, unsigned long long& physical_memory_value);

	void TLB_Eviction(unsigned long long vpn);


	TLBVisualEntry getEntryForVisual(int index) {
		int currentIndex = 0;
		
		for (auto const& [vpn, val] : table) {
			if (currentIndex == index) {
				return { vpn, val.physical_memory, true };
			}
			currentIndex++;
		}
		return { 0, 0, false }; 
	}

	
	int getVPNIndex(unsigned long long vpn) {
		int index = 0;
		for (auto const& [key, val] : table) {
			if (key == vpn) return index;
			index++;
		}
		return -1; 
	}
};
