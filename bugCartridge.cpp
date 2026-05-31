//
// Created by felipe on 22/03/26.
//

#include "bugCartridge.h"

#include <cstring>
#include <fstream>

bugCartridge::bugCartridge() = default;
bugCartridge::~bugCartridge() = default;

std::vector<uint8_t> bugCartridge::ReadAllBytes(const std::string &path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        throw std::runtime_error("Failed to open file: " + path);

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read file");

    return buffer;
}

void bugCartridge::setHeaderData() {
    prgRom_Size = Header[4];
    chrRom_Size = Header[5];
    nametableArr = (Header[6] & 0x1);
}

void bugCartridge::insertCartridge() {
    auto HeaderedROM = ReadAllBytes(filepath);

    std::memcpy(Header.data(), HeaderedROM.data(), 0x10);
    setHeaderData();  // must be before we use prgRom_Size

    size_t prgSize = prgRom_Size * 0x4000;

    // Always copy PRG ROM using the actual size from the header
    std::memcpy(ROM.data(), HeaderedROM.data() + 0x10, prgSize);

    // Only copy CHR ROM if the cart actually has it
    if (chrRom_Size > 0) {
        size_t chrSize = chrRom_Size * 0x2000;
        std::memcpy(chrMem.data(), HeaderedROM.data() + 0x10 + prgSize, chrSize);
    } else {
        // CHR RAM — zero it out so writes start from a clean state
        chrMem.fill(0);
    }
}

void bugCartridge::cpuRead(uint16_t address, uint8_t &data) {
    data = ROM[address - 0x8000];
}

void bugCartridge::ppuRead(uint16_t address, uint8_t &data) {
    data = chrMem[address];
}

void bugCartridge::ppuWrite(uint16_t address, uint8_t data) {
    chrMem[address] = data;
}
