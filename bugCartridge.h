//
// Created by felipe on 22/03/26.
//

#ifndef BUGEMU_BUGCARTRIDGE_H
#define BUGEMU_BUGCARTRIDGE_H

#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <vector>

class bugCartridge {
public:
    bugCartridge();
    ~bugCartridge();

    std::vector<uint8_t> ReadAllBytes(const std::string& path);
    std::array<uint8_t, 0x6000> prgROM{};
    std::array<uint8_t, 0x2000> chrMem{};
    std::array<uint8_t, 0x8000> ROM{};
    std::array<uint8_t, 0x10> Header{};

    // Header data per iNES file format
    uint8_t prgRom_Size {};     // Stored in 16KB units
    uint8_t chrRom_Size {};     // Stored in 8KB units, if 0 that means this cartridge uses CHR RAM, which means it can be written to.
    bool nametableArr {};    // Nametable arrangement 0 = vertical arrangement, 1 = horizontal arrangement
    // We set all of this data when inserting the cartdrige
    void setHeaderData();

    void insertCartridge();
    std::string filepath;

    void cpuRead(uint16_t address, uint8_t &data);
    void ppuRead(uint16_t address, uint8_t &data);
    void ppuWrite(uint16_t address, uint8_t data);
};


#endif //BUGEMU_BUGCARTRIDGE_H