#include "Page_frame.h"


Page_table::Page_table(FileConfiguration& config) :mind_palace(config) {
	ram_read_count = 0;
	ram_write_count = 0;
		frame_count = 0;
		max_frames = floor((config.physical_size / config.page_size));
		dirty_evictions = 0;
	}
	unsigned long long Page_table::getPhysicalAddress(unsigned long long vpn, unsigned long long offset, bool for_writing, TLB& l1, bool hit, char w ) {// has all insertion and replacement policies for the page table
		if (!hit) { ram_read_count++; }
		ofstream logging("log.txt",ios::app);
		if (table.find(vpn) == table.end() && frame_count < max_frames) {
			
			ram_read_count++; // 1. reading the page from DISK to RAM
			Page_frame_node frame;
			frame.dirty_bit = for_writing;
			frame.valid_bit = true;
			frame.physical_memory = frame_count;
			table[vpn] = frame;
			frame_count++;
			LRU_queue.push_front(vpn);
			LRU_map[vpn] = LRU_queue.begin();
			if (for_writing) {
				mind_palace.write(frame.physical_memory, offset, w);
				ram_write_count++;
			}
			else {
			mind_palace.read(frame.physical_memory, offset);

			}
			
			return frame.physical_memory;
		}
		else if (table.find(vpn) == table.end() && frame_count >= max_frames) {
			ram_read_count++;
			unsigned long long least_recently_used_vpn = LRU_queue.back();
			list<unsigned long long >::iterator temp = LRU_map.at(least_recently_used_vpn);
			unsigned long long page_to_rerefer = table.at(least_recently_used_vpn).physical_memory;
			this->lastEvictedFrame = (int)page_to_rerefer;
			
			if (table.at(least_recently_used_vpn).dirty_bit == true) {
				if (logging.is_open()) {
					logging << "--- EVICTION NOTICE: VPN " << least_recently_used_vpn << " was DIRTY. Writing back to Disk" << endl;
				}
				ofstream outfile("HardDisk.txt", ios::app);
				if (outfile.is_open()) {
					for (unsigned long long i = 0; i < mind_palace.frames_size; i++) {
						outfile << mind_palace.read(page_to_rerefer, i);
					}
					outfile << endl;
					outfile.close();
				}
				dirty_evictions++;
			}
			else {
				if (logging.is_open()) {
					logging << "--- EVICTION NOTICE: VPN " << least_recently_used_vpn<< " was CLEAN. Discarding ---" << endl;
				}

				
			}
			
			LRU_queue.erase(temp);
			LRU_map.erase(least_recently_used_vpn);
			table.erase(least_recently_used_vpn);
			l1.TLB_Eviction(least_recently_used_vpn);
			Page_frame_node frame;
			frame.physical_memory = page_to_rerefer;
			frame.dirty_bit = for_writing;
			table[vpn] = frame;
			if (for_writing) {
				mind_palace.write(page_to_rerefer, offset, w);
				ram_write_count++;
			}
			else {

				
				mind_palace.read(page_to_rerefer, offset);
			}
			LRU_queue.push_front(vpn);
			LRU_map[vpn] = LRU_queue.begin();
			return table.at(vpn).physical_memory;

		}

		else if (table.find(vpn) != table.end()) {
			list<unsigned long long>::iterator temp = LRU_map.at(vpn);
			LRU_queue.erase(temp);
			LRU_queue.push_front(vpn);
			LRU_map[vpn] = LRU_queue.begin();

			if (for_writing) {
				mind_palace.write(table[vpn].physical_memory, offset, w);
				ram_write_count++;
			}
			else {
				mind_palace.read(table[vpn].physical_memory, offset);
				
			}
			table[vpn].dirty_bit = table[vpn].dirty_bit || for_writing;

			return table[vpn].physical_memory;

		}
		else {
			return -1;
		}


	}
	int Page_table::getFrame_count() {

		return frame_count;
	}

	void Page_table:: display() {

		for (auto it = table.begin(); it != table.end(); ++it) {
			cout << "VPN " << it->first << "\t" << "Physical frames = " << it->second.physical_memory << endl;
		}
	}

	void Page_table::displayRam() {
		mind_palace.display();
	}


	unsigned long long Page_table::OPT_algorthim_for_getting_pshycal_number(unsigned long long vpn, unsigned long long offset, bool for_writing, TLB& l1,  vector<unsigned long long>& raw_adresses, size_t index, char w ) {
		
		if (table.find(vpn) != table.end() ) {
			if (for_writing) {
				mind_palace.write(table[vpn].physical_memory, offset, w);
				ram_write_count++;
				table[vpn].dirty_bit = table[vpn].dirty_bit || for_writing;
			}
			else { 
				mind_palace.read(table[vpn].physical_memory, offset);
				}
			return table.at(vpn).physical_memory; //this is when we already have the vpn in page table and no switiching is needed
		}
		if (table.find(vpn) == table.end()) {// this means that frame is not in the memeory and needs to be called
			if (frame_count < max_frames) {
				ram_read_count++;
				Page_frame_node temp;
				temp.physical_memory = frame_count;
				temp.dirty_bit = for_writing;
				frame_count++;
				table[vpn]=temp;
				if (for_writing) {
				mind_palace.write(temp.physical_memory, offset, w);
				ram_write_count++;
				}
				else {
					mind_palace.read(temp.physical_memory, offset);
					 }
			}
			else {

				auto i = table.begin();
				FileConfiguration config;
				unsigned long long max_vpn=table.begin()->first;
				int max_distance = 0;
				for (auto i = table.begin(); i != table.end(); i++) {
					int count = 0;
					unsigned long long current_vpn_in_table = i->first;
					size_t distance;
					bool found = false;
					for (size_t temp = index;  temp < lookahead_buffer.size();temp++) {
						if (lookahead_buffer[temp] == current_vpn_in_table) {
							distance = temp;
							found = true;
							break;
						}
					}
					if (found == false) {
						max_vpn = current_vpn_in_table;
						break;
					}
					else if (distance > max_distance) {
						max_distance = distance;
						max_vpn = current_vpn_in_table;
					}
				}
				this->lastEvictedFrame = (int)table[max_vpn].physical_memory;
				ofstream logging("OPT_logging", ios::app);
				if (table[max_vpn].dirty_bit) {
			
					if (logging.is_open()) {
						logging << "--- EVICTION NOTICE: VPN " << max_vpn << " was DIRTY. Writing back to Disk" << endl;
					}
					ofstream outfile("HardDisk.txt", ios::app);
					if (outfile.is_open()) {
						for (unsigned long long i = 0; i < mind_palace.frames_size; i++) {
							outfile<<mind_palace.read(table[max_vpn].physical_memory,  i);
						}
						outfile << endl;
						outfile.close();
					}
					dirty_evictions++;
				}
					
				else {
					if (logging.is_open()) {
						logging << "--- EVICTION NOTICE: VPN " << max_vpn << " was CLEAN. Discarding ---" << endl;
					}
				
				}
				Page_frame_node temp;
				ram_read_count++;
				temp.physical_memory = table[max_vpn].physical_memory;
				table.erase(max_vpn);
				l1.TLB_Eviction(max_vpn);
				temp.dirty_bit = for_writing;
				table[vpn] = temp;
				if (for_writing) {
					ram_write_count++;
					mind_palace.write(temp.physical_memory, offset, w);
				}
				else {
					
					mind_palace.read(temp.physical_memory, offset);
				}
			}
		}
		return table[vpn].physical_memory;
		
	}

	void Page_table::LOOKAHEADINSERTION(FileConfiguration &config) { 
		
		
		for (auto temp = config.actual_adress.begin(); temp != config.actual_adress.end(); temp++) {
			lookahead_buffer.push_back(config.getVirtual_page_number(*temp));
		}		
	}

	unsigned long long Page_table::accessPageTableDirectly(unsigned long long vpn, unsigned long long offset, bool for_writing, char w) {
		auto it = table.find(vpn);
		if (it != table.end()) {
			
			if (for_writing) {
				it->second.dirty_bit = true;
				mind_palace.write(it->second.physical_memory, offset, w);
				ram_write_count++;
			}
			else {
				
				mind_palace.read(it->second.physical_memory, offset);
			}
			return it->second.physical_memory;
		}
		return -1;
	}



	Page_table::~Page_table() {
		cout << "\n FINAL SYSTEM SYNC (Flushing Remaining Dirty Pages) " << endl;

		ofstream outfile("HardDisk.txt", ios::app);
		if (outfile.is_open()) {
			for (unordered_map<unsigned long long, Page_frame_node> ::iterator temp = table.begin(); temp != table.end(); temp++) {
				if (temp->second.dirty_bit == true) {

					cout << "The hard disk is being updated by the Runner at virtual address " << temp->first << " and physical frame " << temp->second.physical_memory << "due to being dirty\n";
					for (unsigned long long i = 0; i < mind_palace.frames_size; i++) {

						outfile << mind_palace.read(temp->second.physical_memory, i);
					}
					outfile << endl;
				}
			}
		}
	}


	unsigned long long Page_table::acccess_using_FIFO(unsigned long long vpn, unsigned long long offset, bool for_writing, TLB& l1, char w) {
		if (table.find(vpn) != table.end()) {
			ram_read_count++;
			if (for_writing) {
				mind_palace.write(table[vpn].physical_memory, offset, w);
				ram_write_count++;
				table[vpn].dirty_bit = table[vpn].dirty_bit || for_writing;
			}
			else {
				mind_palace.read(table[vpn].physical_memory, offset);
			}
			return table[vpn].physical_memory;

		}
		else {
			if (frame_count < max_frames) {
				ram_read_count++;
				FIFO_queue.push_back(vpn);
				Page_frame_node temp;
				temp.physical_memory = frame_count;

				frame_count++;
				table[vpn] = temp;
				if (for_writing) {
					table[vpn].dirty_bit = true;
					mind_palace.write(table[vpn].physical_memory, offset, w);
					ram_write_count++;
				}
				else {
					mind_palace.read(table[vpn].physical_memory, offset);
				}
				return table[vpn].physical_memory;
			}
			else {//the TLB is full and we need to evict a page using FIFO
				ram_read_count++;
				unsigned long long page_to_evict_vpn = FIFO_queue.front();
				
				FIFO_queue.pop_front();
				unsigned long long frame = table[page_to_evict_vpn].physical_memory;
				this->lastEvictedFrame = (int)frame;
				ofstream logging("FIFO_logging.txt", ios::app);
				if (table[page_to_evict_vpn].dirty_bit) {
					if (table[page_to_evict_vpn].dirty_bit == 1) {
						ofstream outfile("HardDisk.txt", ios::app);
						if (logging.is_open()) {
							logging << "EVICTION NOTICE : VPN " << page_to_evict_vpn << " was DIRTY.Writing back to Disk" << endl;
							if (outfile.is_open()) {
								for (unsigned long long i = 0; i < mind_palace.frames_size; i++) {
									outfile << mind_palace.read(table[page_to_evict_vpn].physical_memory, i);
								}
								outfile << endl;
								outfile.close();
							}
							dirty_evictions++;
						}
					}
				}
				else {
					ofstream logging("FIFO_logging.txt", ios::app);
					if (logging.is_open()) {
						logging << "EVICTION NOTICE : VPN " << page_to_evict_vpn << " was clean no need to write back to Disk" << endl;
					}
				}

				l1.TLB_Eviction(page_to_evict_vpn);
				table.erase(page_to_evict_vpn);
				Page_frame_node temp;
				temp.physical_memory = frame;
				table[vpn] = temp;
				if (for_writing) {
					table[vpn].dirty_bit = true;
					mind_palace.write(frame, offset, w);
					ram_write_count++;
				}
				else {
					mind_palace.read(frame, offset);
				}
				FIFO_queue.push_back(vpn);

				return table[vpn].physical_memory;
			}

		}
	}
	
	
	


