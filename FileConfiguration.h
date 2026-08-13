#pragma once
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <json/json.h>

using namespace std;

struct FileConfiguration {
    unsigned long long physical_size;
    unsigned long long page_size;
    unsigned long long TLB_size;
    unsigned long long virtual_size;
    unsigned long long latency;
    vector<unsigned long long> actual_adress;

    int No_of_bits_to_be_shifted();
    unsigned long long getVirtual_page_number(unsigned long long address);
    unsigned long long offset(unsigned long long address);
    void FileParser();
    void Display();
    void Reading_Addresses();
    void Writing();

};