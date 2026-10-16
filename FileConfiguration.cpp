#include "FileConfiguration.h"
#include <stdexcept>

int FileConfiguration::No_of_bits_to_be_shifted() {
    int bits = 0;
    for (auto value = page_size; value > 1; value >>= 1) ++bits;
    return bits;
}
unsigned long long FileConfiguration::getVirtual_page_number(unsigned long long address) {
    if (address > virtual_size) throw std::out_of_range("Virtual address exceeds configured maximum");
    return address >> No_of_bits_to_be_shifted();
}
unsigned long long FileConfiguration::offset(unsigned long long address) {
    if (address > virtual_size) throw std::out_of_range("Virtual address exceeds configured maximum");
    return address & (page_size - 1);
}
void FileConfiguration::FileParser() {
    std::ifstream file("Parameters.json");
    if (!file) throw std::runtime_error("Cannot open Parameters.json in working directory");
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    if (!Json::parseFromStream(builder, file, &root, &errors))
        throw std::runtime_error("Invalid Parameters.json: " + errors);
    for (const char* key : {"physical_size", "page_size", "TLB_size", "virtual_size", "latency"})
        if (!root.isMember(key) || !root[key].isUInt64())
            throw std::runtime_error(std::string("Missing or invalid unsigned integer: ") + key);
    physical_size = root["physical_size"].asUInt64();
    page_size = root["page_size"].asUInt64();
    TLB_size = root["TLB_size"].asUInt64();
    virtual_size = root["virtual_size"].asUInt64();
    latency = root["latency"].asUInt64();
    if (page_size == 0 || (page_size & (page_size - 1)) != 0)
        throw std::runtime_error("page_size must be a positive power of two");
    if (physical_size < page_size || physical_size % page_size != 0 || TLB_size == 0)
        throw std::runtime_error("RAM must contain whole pages and TLB_size must be positive");
}
void FileConfiguration::Display() {
    std::cout << "Physical Size: " << physical_size << "\nPage Size: " << page_size
              << "\nTLB Size: " << TLB_size << "\nVirtual maximum address: " << virtual_size
              << "\nLatency: " << latency << '\n';
}
void FileConfiguration::Reading_Addresses() {
    std::ifstream file("addresses.txt");
    if (!file) throw std::runtime_error("Cannot open addresses.txt");
    actual_adress.clear();
    std::string token;
    while (file >> token) {
        if (token.empty() || token.front() == '-') throw std::runtime_error("Invalid address: " + token);
        size_t used = 0;
        auto address = std::stoull(token, &used, 16);
        if (used != token.size() || address > virtual_size)
            throw std::runtime_error("Invalid/out-of-range address: " + token);
        actual_adress.push_back(address);
    }
    if (actual_adress.empty()) throw std::runtime_error("addresses.txt is empty");
}
void FileConfiguration::Writing() {
    std::ofstream file("addresses.txt");
    if (!file) throw std::runtime_error("Cannot write addresses.txt");
    file << std::hex << 02123443 << '\n';
}
