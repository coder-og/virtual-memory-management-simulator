                                   #pragma once
#include <unordered_map>
#include <list>
#include <fstream>
#include <iostream>
#include <cmath>
#include "Ram_memory.h"
#include "TLB_table.h"
#include<vector>

using namespace std;

class Page_table {
private:
	unordered_map<unsigned long long, Page_frame_node> table;
	list<unsigned long long >FIFO_queue;
	
	list<unsigned long long> LRU_queue;
	unordered_map<unsigned long long, list<unsigned long long>::iterator> LRU_map;
	Ram_memory mind_palace;
	vector<unsigned long long> lookahead_buffer;

public:
	int frame_count;
	int max_frames;
	int lastEvictedFrame = -1;
	unsigned long long dirty_evictions;
	unsigned long long ram_read_count;
	unsigned long long ram_write_count;

	Page_table(FileConfiguration& config);

	unsigned long long getPhysicalAddress(unsigned long long vpn, unsigned long long offset, bool for_writing, TLB& l1, bool hit, char w = 'W');

	int getFrame_count();
	void display();
	void displayRam();
	void LOOKAHEADINSERTION(FileConfiguration& config);
	bool isFrameDirty(int frameIndex) {
		
		for (auto const& [vpn, node] : table) {
			if (node.physical_memory / 4096 == frameIndex) {
				return node.dirty_bit;
			}
		}
		return false;
	}
	unsigned long long OPT_algorthim_for_getting_pshycal_number(unsigned long long vpn, unsigned long long offset, bool for_writing, TLB& l1,vector<unsigned long long> &raw_adresses, size_t index, char w = 'W');
	unsigned long long accessPageTableDirectly(unsigned long long vpn, unsigned long long offset, bool for_writing, char w);
	unsigned long long acccess_using_FIFO(unsigned long long vpn, unsigned long long offset, bool for_writing, TLB& l1, char w = 'W');
	~Page_table();
};