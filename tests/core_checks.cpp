#include "Page_frame.h"
#include <stdexcept>
void check(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
int main() {
    FileConfiguration config{};
    config.page_size = 4096; config.physical_size = 8192; config.TLB_size = 2;
    config.virtual_size = 65535; config.latency = 10;
    check(config.getVirtual_page_number(0x1234) == 1, "VPN decomposition");
    check(config.offset(0x1234) == 0x234, "Offset decomposition");
    TLB cache(config); unsigned long long frame = 0;
    cache.insert_with_LRU_algo(1, 0, true); cache.insert_with_LRU_algo(2, 1, true);
    check(cache.find(1, frame) && frame == 0, "LRU hit");
    cache.insert_with_LRU_algo(3, 1, true);
    check(!cache.find(2, frame), "LRU evicts least recently used");
    cache.TLB_Eviction(1); check(!cache.find(1, frame), "TLB invalidation");
    TLB fifo(config); Page_table pages(config);
    check(pages.acccess_using_FIFO(1, 0, true, fifo) == 0, "First frame");
    check(pages.acccess_using_FIFO(2, 0, true, fifo) == 1, "Second frame");
    check(pages.isFrameDirty(1), "Dirty state uses frame index, not byte address");
    check(pages.acccess_using_FIFO(3, 0, false, fifo) == 0, "FIFO reuses oldest frame");
    check(pages.dirty_evictions == 1, "Dirty eviction accounting");
    check(!pages.isFrameDirty(0), "New read-only page is clean");
    std::cout << "PASS: translation, TLB LRU/invalidation, FIFO dirty eviction\n";
}
