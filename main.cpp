#include <iostream>
#include <cstdint>
#include <iomanip>
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

std::string filepath = "C:/Users/USER UNAL/Documents/GitHub/BugEmu/test_roms/2_ReadWrite.nes";

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

void Write(uint16_t Address, uint8_t Value) {
    if (Address < 0x800) {
        RAM[Address] = Value;
    }
    if (Address <= 0x1FFF) {
        constexpr uint16_t MASK = 0x7FF;
        RAM[Address & MASK] = Value;
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
            case 0xA4: { //LDY Zero Page
                uint8_t Addr = Read(ProgramCounter);
                ProgramCounter++;
                Y = Read(Addr);
                cycle = 0;
                break;
            }
            case 0xAC: { //LDY Absolute
                uint8_t Low = Read(ProgramCounter);
                ProgramCounter++;
                uint8_t High = Read(ProgramCounter);
                ProgramCounter++;
                Y = Read((High << 8) | Low);
                cycle = 0;
                break;
            }
            case 0xA2:  //LDX Immediate
                X = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA6: { //LDX Zero Page
                uint8_t Addr = Read(ProgramCounter);
                ProgramCounter++;
                X = Read(Addr);
                cycle = 0;
                break;
            }
            case 0xAE: { //LDX Absolute
                uint8_t Low = Read(ProgramCounter);
                ProgramCounter++;
                uint8_t High = Read(ProgramCounter);
                ProgramCounter++;
                X = Read((High << 8) | Low);
                cycle = 0;
                break;
            }
            case 0xA9:  //LDA Immediate
                A = Read(ProgramCounter);
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA5: { //LDA Zero Page
                uint8_t Addr = Read(ProgramCounter);
                ProgramCounter++;
                A = Read(Addr);
                cycle = 0;
                break;
            }
            case 0xAD: { //LDA Absolute
                uint8_t Low = Read(ProgramCounter);
                ProgramCounter++;
                uint8_t High = Read(ProgramCounter);
                ProgramCounter++;
                A = Read((High << 8) | Low);
                cycle = 0;
                break;
            }
            case 0x85: {
                //STA Zero Page
                uint8_t Temp = Read(ProgramCounter);
                ProgramCounter++;
                Write(Temp, A);
                cycle = 0;
                break;
            }
            case 0x8D: {
                //STA Absolute
                uint8_t Temp_Low = Read(ProgramCounter);
                ProgramCounter++;
                uint8_t Temp_High = Read(ProgramCounter);
                ProgramCounter++;
                Write((Temp_High << 8) | Temp_Low, A);
                cycle = 0;
                break;
            }
            case 0x86: {
                //STX Zero Page
                uint8_t Temp = Read(ProgramCounter);
                ProgramCounter++;
                Write(Temp, X);
                cycle = 0;
                break;
            }
            case 0x8E: {
                //STX Absolute
                uint8_t Temp_Low = Read(ProgramCounter);
                ProgramCounter++;
                uint8_t Temp_High = Read(ProgramCounter);
                ProgramCounter++;
                Write((Temp_High << 8) | Temp_Low, X);
                cycle = 0;
                break;
            }
            case 0x84: {
                //STY Zero Page
                uint8_t Temp = Read(ProgramCounter);
                ProgramCounter++;
                Write(Temp, Y);
                cycle = 0;
                break;
            }
            case 0x8C: {
                //STY Absolute
                uint8_t Temp_Low = Read(ProgramCounter);
                ProgramCounter++;
                uint8_t Temp_High = Read(ProgramCounter);
                ProgramCounter++;
                Write((Temp_High << 8) | Temp_Low, Y);
                cycle = 0;
                break;
            }
            default:
                std::cout <<"Unknown opcode: 0x"
                << std::hex
                << std::uppercase
                << std::setw(2)
                << std::setfill('0')
                << static_cast<int>(opcode)
                <<std::endl;
                cycle = 0;
                break;
        }
    }
}
int main() {
    Reset();
    return 0;
}