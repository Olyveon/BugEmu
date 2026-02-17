#include <iostream>
#include <cstdint>
#include <array>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>

uint16_t ProgramCounter;
uint8_t A;
uint8_t X;
uint8_t Y;

std::array<uint8_t, 0x800> RAM{};
std::array<uint8_t, 0x8000> ROM{};
std::array<uint8_t, 0x10> Header{};

std::string filepath = "/home/felipe/CLionProjects/BugEmu/test_roms/1_Example.nes";

void EmulateCPU();
void Run();

std::vector<uint8_t> ReadAllBytes(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        throw std::runtime_error("Failed to open file");

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read file");

    return buffer;
}


uint8_t Read(uint16_t Address) {
    if (Address < 0x800) {
        return RAM[Address];
    }
    if (Address <= 0x1FFF) {
        constexpr uint16_t MASK = 0x7FF;
        return RAM[Address & MASK];
    }
    if (Address >= 0x8000) {
        return ROM[Address - 0x8000];
    }
}

void Reset() {
    auto HeaderedROM = ReadAllBytes(filepath);
    std::memcpy(ROM.data(), HeaderedROM.data() + 0x10,0x8000);
    std::memcpy(Header.data(), HeaderedROM.data(),0x10);
    uint16_t PCL = Read(0xFFFC);
    uint16_t PCH = Read(0xFFFD);
    ProgramCounter = (PCH << 8) | PCL;
    Run();
}

bool CPU_Halted = false;
uint8_t opcode;
int cycle = 0;

void Run() {
    while (!CPU_Halted) {
        EmulateCPU();
    }

}

void EmulateCPU() {
    if (cycle == 0) {
        opcode = Read(ProgramCounter);
        ProgramCounter++;
        cycle++;
    } else {
        switch (opcode) {
            case 0x02:  // HLT
                cycle = 0;
                CPU_Halted = true;
                break;
            case 0xA0:  // Immediate LDY, 2 cycles
                Y = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA2:  // Immediate LDX, 2 cycles
                X = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA9:  // Immediate LDA, 2 cycles
                A = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            default: //Unkwown or unimplemented opcode
                cycle = 0;
                break;
        }
    }
}

int main() {
    Reset();
    return 0;
}