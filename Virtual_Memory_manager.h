#pragma once
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <json/json.h>
#include "FileConfiguration.h"
#include "TLB_table.h"
#include "Page_frame.h"// Important: Ensure this name matches your file

using namespace std;

struct Virtual_Memory_manager {
    void Runner();
    void SPRINTER();
    void SPRINTER_OPT_TLB();   
    void Walker();
};
