#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

uint16_t ProgramCounter;
uint8_t A;
uint8_t X;
uint8_t Y;

std::array<uint8_t, 0x800> RAM{};
std::array<uint8_t, 0x8000> ROM{};
std::array<uint8_t, 0x10> Header{};

std::string filepath = "C:/Users/USER UNAL/Documents/GitHub/BugEmu/test_roms/1_Example.nes";

bool CPU_Halted = false;

void Emulate_CPU();
void Run();

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

std::vector<uint8_t> ReadAllBytes(const std::string& path)
{
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

void Reset() {
    auto HeaderedROM = ReadAllBytes(filepath);
    std::memcpy(ROM.data(), HeaderedROM.data() + 0x10, 0x8000);
    std::memcpy(Header.data(), HeaderedROM.data() + 0x10, 0x10);
    uint8_t PCL = Read(0xFFFC);
    uint8_t PCH = Read(0xFFFD);
    ProgramCounter = (PCH << 8) | PCL;
    Run();
}

void Run() {
    while (!CPU_Halted) {
        Emulate_CPU();
    }
}

int cycle = 0;
uint8_t opcode;

void Emulate_CPU() {
    if (cycle == 0) {
        opcode = Read(ProgramCounter);
        ProgramCounter++;
        cycle++;
    }
    else {
        switch (opcode) {
            case 0x02:  //HTL
                CPU_Halted = true;
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA0:  //LDY Immediate
                Y = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA2:  //LDX Immediate
                X = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA9:  //LDA Immediate
                A = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            default:
                std::cout<<"Unknown opcode: "<<opcode<<std::endl;
                cycle = 0;
                break;
        }
    }
}
int main() {
    Reset();
    return 0;
}