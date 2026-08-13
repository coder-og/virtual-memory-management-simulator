#include "Ram_memory.h"


Page_frame_node::Page_frame_node() {
		dirty_bit = false;
		valid_bit = true;
	}



	Ram_memory:: Ram_memory(FileConfiguration& config) {
		frames_size = config.page_size;
		unsigned long long total_pages = floor((config.physical_size / frames_size));
		memory.resize(total_pages, vector<char>(frames_size, '-'));
	}

	void Ram_memory::write(unsigned long long physical, unsigned long long offset, char data) {
		memory[physical][offset] = data;
	}
	char Ram_memory:: read(unsigned long long physical, unsigned long long offset) {
		return memory[physical][offset];
	}

	void Ram_memory::display() {
		for (int i = 0; i < memory.size(); i++) {
			for (int j = 0; j < memory[i].size(); j++) {
				cout << memory[i][j];
			}
			cout << endl;
		}
	}




