#include "Drawer.h"
#include <string>
int main(int argc, char** argv) {
    Drawer drawer;
    std::string mode = argc > 1 ? argv[1] : "fifo";
    if (mode == "fifo") drawer.Walker1();
    else if (mode == "lru") drawer.Runner1();
    else if (mode == "opt") drawer.SPRINTER_OPT_TLB1();
    else { std::cerr << "Usage: memory_gui [fifo|lru|opt]\n"; return 1; }
}
