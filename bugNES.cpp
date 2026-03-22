//
// Created by felipe on 22/03/26.
//

#include "bugNES.h"

bugNES::bugNES() {
    cpu.ConnectSystem(this);
}

bugNES::~bugNES() = default;

uint8_t bugNES::cpuRead(uint16_t address) {
    if (address < 0x800) {
        return ram[address];
    }
    if (address <= 0x1FFF) {
        constexpr uint16_t MASK = 0x7FF;
        return ram[address & MASK];
    }
    if (address >= 0x8000) {
        uint8_t data = 0x00;
        cart.cpuRead(address, data);
        return data;
    }
    return 0;
}

void bugNES::cpuWrite(uint16_t address, uint8_t value) {
    if (address < 0x800) {
        ram[address] = value;
    }
    if (address <= 0x1FFF) {
        constexpr uint16_t MASK = 0x7FF;
        ram[address & MASK] = value;
    }
}

void bugNES::nesClock() {
    ppu.ppuClock();

    if (SystemCounter % 3 == 0) {
        cpu.clock();
    }

    SystemCounter++;
}
