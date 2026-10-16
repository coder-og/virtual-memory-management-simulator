#include "Drawer.h"
#include "mem_visualization.h"

using namespace std;

// 1. Standard simulation using LRU
void Drawer::Runner1() {
    ofstream clear_disk("HardDisk.txt", ios::trunc);
    FileConfiguration config;
    config.FileParser();
    config.Reading_Addresses();

    TLB cache(config);
    Page_table pages(config);
    MemoryVisualizer gui(1200, 800);

    unsigned long long frame_address;
    int instruction_count = 0;
    auto it = config.actual_adress.begin();
    bool hitStatus = false;

    while (gui.IsRunning()) {
        int activeIndex = -1;

        if (it != config.actual_adress.end()) {
           
            cache.lastEvictedSlot = -1;
            pages.lastEvictedFrame = -1;

            unsigned long long address = *it;
            unsigned long long vpn = config.getVirtual_page_number(address);
            unsigned long long offset = config.offset(address);

            bool is_write = (instruction_count % 5 == 0);
            char x = is_write ? 'W' : 'R';

            hitStatus = cache.find(vpn, frame_address);
            if (hitStatus) {
                pages.getPhysicalAddress(vpn, offset, is_write, cache, hitStatus, x);
            }
            else {
               
                unsigned long long p_frame = pages.getPhysicalAddress(vpn, offset, is_write, cache, false, x);
               
                cache.insert_with_LRU_algo(vpn, p_frame, true);
            }

        
            activeIndex = cache.getVPNIndex(vpn);

            it++;
            instruction_count++;

            if (config.latency > 0) {
                WaitTime((float)config.latency / 1000.0f);
            }
        }

        gui.Render(cache, pages, hitStatus, config, activeIndex, "LRU Replacement");
    }
    cache.perfomance_metrics(config.latency, pages.ram_read_count, pages.ram_write_count, pages.dirty_evictions, "LRU_GUI");
}

// 2. Optimal (OPT) Algorithm simulation


// 3. FIFO Algorithm simulation
void Drawer::Walker1() {
    ofstream clear_disk("HardDisk.txt", ios::trunc);
    FileConfiguration config;
    config.FileParser();
    config.Reading_Addresses();

    TLB cache(config);
    Page_table pages(config);
    MemoryVisualizer gui(1200, 800);

    unsigned long long frame_address;
    int instruction_count = 0;
    auto it = config.actual_adress.begin();
    bool hitStatus = false;

    while (gui.IsRunning()) {
        int activeIndex = -1;

        if (it != config.actual_adress.end()) {
            cache.lastEvictedSlot = -1;
            pages.lastEvictedFrame = -1;

            unsigned long long address = *it;
            unsigned long long vpn = config.getVirtual_page_number(address);
            unsigned long long offset = config.offset(address);

            bool is_write = (instruction_count % 5 == 0);
            char x = is_write ? 'W' : 'R';

            hitStatus = cache.findFIFOTLB(vpn, frame_address);
            if (hitStatus) {
                pages.accessPageTableDirectly(vpn, offset, is_write, x);
            }
            else {
                unsigned long long p_frame = pages.accessPageTableDirectly(vpn, offset, is_write, x);
                if (p_frame == (unsigned long long) - 1) {
                    p_frame = pages.acccess_using_FIFO(vpn, offset, is_write, cache, x);
                }
                cache.Insert_with_FIFO_algo(vpn, p_frame, true);
            }

            activeIndex = cache.getVPNIndex(vpn);
            it++;
            instruction_count++;

            if (config.latency > 0) {
                WaitTime((float)config.latency / 1000.0f);
            }
        }

        gui.Render(cache, pages, hitStatus, config, activeIndex, "FIFO Replacement");
    }
    cache.perfomance_metrics(config.latency, pages.ram_read_count, pages.ram_write_count, pages.dirty_evictions, "FIFO_GUI");
}


void Drawer::SPRINTER_OPT_TLB1() {
    ofstream clear_disk("HardDisk.txt", ios::trunc);
    ofstream clear_disk2("OPT_logging", ios::trunc);
    FileConfiguration config;
    config.FileParser();
    config.Reading_Addresses();

    TLB cache(config);
    Page_table pages(config);

    cache.OPT_LOOKAHEAD_INSERTION(config);
    pages.LOOKAHEADINSERTION(config);

    MemoryVisualizer gui(1200, 800);

    unsigned long long frame_address;
    int instruction_count = 0;
    size_t index = 0;
    auto it = config.actual_adress.begin();
    bool hitStatus = false;

    while (gui.IsRunning()) {
        int activeIndex = -1;

        if (it != config.actual_adress.end()) {
            cache.lastEvictedSlot = -1;
            pages.lastEvictedFrame = -1;

            unsigned long long address = *it;
            unsigned long long vpn = config.getVirtual_page_number(address);
            unsigned long long offset = config.offset(address);

            bool is_write = (instruction_count % 5 == 0);
            char x = is_write ? 'W' : 'R';

            hitStatus = cache.findOPT_TLB(vpn, frame_address);
            if (hitStatus) {
                pages.accessPageTableDirectly(vpn, offset, is_write, x);
            }
            else {
                unsigned long long p_frame = pages.accessPageTableDirectly(vpn, offset, is_write, x);
                if (p_frame == (unsigned long long)-1) {
                    p_frame = pages.OPT_algorthim_for_getting_pshycal_number(vpn, offset, is_write, cache, config.actual_adress, index, x);
                }
                cache.insert_with_OPT_algo(vpn, p_frame, true, index);
            }

            activeIndex = cache.getVPNIndex(vpn);
            it++;
            index++;
            instruction_count++;

            if (config.latency > 0) {
                WaitTime((float)config.latency / 1000.0f);
            }
        }
        gui.Render(cache, pages, hitStatus, config, activeIndex, "Full OPT (TLB + RAM)");
    }
    cache.perfomance_metrics(config.latency, pages.ram_read_count, pages.ram_write_count, pages.dirty_evictions, "OPT_GUI");
}
