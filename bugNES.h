/*
 *  This acts as a system/bus class to coordinate between
 *  the cpu, the ppu, the apu, any mappers and the cartridge/data itself
 *  I probably should have done this sooner but now it's better than never
 *  todo: finish tidying up functions that should be here and not in bugCPU
 */

#ifndef BUGEMU_BUGNES_H
#define BUGEMU_BUGNES_H

#pragma once
#include <array>
#include <cstdint>
#include "bugCpu.h"
#include "bugPpu.h"
#include "bugCartridge.h"

class bugNES {
public:
    bugNES();
    ~bugNES();

    // devices connected to the system
    std::array<uint8_t, 0x800> ram;
    bugCpu cpu;
    bugPpu ppu;
    bugCartridge cart;


    void cpuWrite(uint16_t address, uint8_t data);
    uint8_t cpuRead(uint16_t address);


    void nesClock();

private:
    int SystemCounter {};   // Measures the number of ticks, with 1 system tick = 1 PPU cycle
};


#endif //BUGEMU_BUGNES_H