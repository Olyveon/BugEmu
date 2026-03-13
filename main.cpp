#include <string>

#include"bugCpu.h"

int main() {
    bugCpu cpu;
    cpu.filepath = "C:/Users/felip/CLionProjects/BugEmu/test_roms/6_Instructions2.nes";
    cpu.reset();
    return 0;
}