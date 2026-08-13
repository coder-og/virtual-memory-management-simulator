#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include "FileConfiguration.h"

using namespace std;

class TLB;

struct Page_frame_node {
	bool dirty_bit;
	bool valid_bit;
	unsigned long long physical_memory;
	Page_frame_node();
	std::vector<Page_frame_node> getFramesState() {
		std::vector<Page_frame_node> state; // Assuming 10 frames
		// Loop through your internal page table/frame data 
		// and fill this vector based on your current RAM state
		return state;
	}
};

struct Ram_memory {
	vector<vector<char>> memory;
	unsigned long long frames_size;

	Ram_memory(FileConfiguration& config);

	void write(unsigned long long physical, unsigned long long offset, char data);
	char read(unsigned long long physical, unsigned long long offset);
	void display();
};