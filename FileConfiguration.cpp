#include "FileConfiguration.h"



    int FileConfiguration:: No_of_bits_to_be_shifted() {
        float power = log2(page_size);
        power = ceil(power);
        return int(power);
    }

    unsigned long long FileConfiguration::getVirtual_page_number(unsigned long long address) {
        if (address <= virtual_size) {
            unsigned long long shifted = No_of_bits_to_be_shifted();
            address = address >> shifted;
            return address;
        }
        return 0; 
    }


    unsigned long long FileConfiguration::offset(unsigned long long address) {
        if (address <= virtual_size) {
            address = address & (page_size - 1);
            return address;
        }
        return 0;


    }

    void FileConfiguration::FileParser() {
        ifstream file("Parameters.json");

        if (file.is_open()) {
            Json::Value root;
            Json::CharReaderBuilder builder;
            string errors;

            if (Json::parseFromStream(builder, file, &root, &errors)) {
                physical_size = root["physical_size"].asUInt64();
                page_size = root["page_size"].asUInt64();
                TLB_size = root["TLB_size"].asUInt64();
                virtual_size = root["virtual_size"].asUInt64();
                latency = root["latency"].asUInt64();
            }
            else {
                cout << "Failed to parse JSON: " << errors << endl;
            }
        }


    }

    void FileConfiguration:: Display() {
        cout << "Physical Size: " << physical_size << endl;
        cout << "Page Size: " << page_size << endl;
        cout << "TLB Size: " << TLB_size << endl;
        cout << "Virtual Size: " << virtual_size << endl;
        cout << "Latency: " << latency << endl;
    }


    void FileConfiguration:: Reading_Addresses() {
        ifstream infile("addresses.txt");
        unsigned long long address;
        if (infile.is_open()) {
            while (infile >> hex >> address) {
                actual_adress.push_back(address);
            }
        }
        else {
            cout << "File cant be opened";
        }
    }
    void FileConfiguration::Writing() {
        ofstream infile("addresses.txt");
        unsigned long long address;
        if (infile.is_open()) {
            infile << 02123443;
        }
        else {
            cout << "File cant be opened";
        }
    }


