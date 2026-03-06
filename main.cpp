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
uint8_t stackPointer;

bool flag_Carry;
bool flag_Zero;
bool flag_InterruptDisable;
bool flag_Decimal;
bool flag_Overflow;
bool flag_Negative;

std::array<uint8_t, 0x800> RAM{};
std::array<uint8_t, 0x8000> ROM{};
std::array<uint8_t, 0x10> Header{};


//std::string filepath = "/home/felipe/CLionProjects/BugEmu/test_roms/4_TheStack.nes";
std::string filepath = "C:/Users/felip/CLionProjects/BugEmu/test_roms/5_Instructions1.nes";

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

void Push(uint8_t Value) {
    Write((0x100 | stackPointer), Value);
    stackPointer --;
}

uint8_t Pull() {
    stackPointer++;
    uint8_t Temp = Read((0x100 | stackPointer));
    return Temp;
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
    flag_InterruptDisable = true;
    stackPointer = 0xFD;
    Run();
}

void Run() {
    while (!CPU_Halted) {
        Emulate_CPU();
    }
}

int cycle = 0;
uint8_t opcode;
uint8_t Low;
uint8_t High;
uint8_t Temp;
int cycles;

//TODO: Add way to check if high byte of PC has changed thus requiring one more CPU Cycle (page crossing)

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
            case 0x10: // BPL, Branch on Plus
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;

                if (!flag_Negative) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0x30: //BMI Branch on Minus
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                if (flag_Negative) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0x50: //BVC Branch on Overflow Clear
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                if (!flag_Overflow) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0x70: //BVS Branch on Overflow Set
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                if (flag_Overflow) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0x90: //BCC Branch on Carry Clear
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;

                if (!flag_Carry) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0xB0: //BCS Branch on Carry Set
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                if (flag_Carry) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0xD0: //BNE Branch on Not Equal (not zero)
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                if (!flag_Zero) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0xF0: //BNQ Branch on Equal (zero)
            {
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                if (flag_Zero) {
                    if (Temp > 127) {
                        Temp -= 256;
                    }
                    ProgramCounter = Temp + ProgramCounter;
                    cycle = 0;
                } else {
                    cycle = 0;
                }
                break;
            }
            case 0x48: //PHA Push A
                Push(A);
                cycle = 0;
                break;
            case 0x68: //PLA Pull A
                A = Pull();
                flag_Zero = A == 0;
                flag_Negative = A >= 0x80;
                cycle = 0;
                break;
            case 0x20: //JSR
            {
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                Push(ProgramCounter >> 8);  //Push PCH
                Push(static_cast<uint8_t>(ProgramCounter)); //Push PCL
                ProgramCounter = (High << 8) | Low;
                cycle = 0;
                break;
            }
            case 0x4C: //JMP (absolute addressing)
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                ProgramCounter = (High << 8) | Low;
                cycle = 0;
                break;
            case 0x60: //RTS
            {
                Low = Pull();
                High = Pull();
                ProgramCounter = (High << 8) | Low;
                ProgramCounter++;
                cycle = 0;
                break;
            }
            case 0xA0:  //LDY Immediate
                Y = Read(ProgramCounter);
                flag_Zero = Y == 0;
                flag_Negative = Y > 127;
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA4: { //LDY Zero Page
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                Y = Read(Temp);
                flag_Zero = Y == 0;
                flag_Negative = Y > 127;
                cycle = 0;
                break;
            }
            case 0xAC: { //LDY Absolute
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                ProgramCounter++;
                Y = Read((High << 8) | Low);
                flag_Zero = Y == 0;
                flag_Negative = Y > 127;
                cycle = 0;
                break;
            }
            case 0xA2:  //LDX Immediate
                X = Read(ProgramCounter);
                flag_Zero = X == 0;
                flag_Negative = X > 127;
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA6: { //LDX Zero Page
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                X = Read(Temp);
                flag_Zero = X == 0;
                flag_Negative = X > 127;
                cycle = 0;
                break;
            }
            case 0xAE: { //LDX Absolute
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                ProgramCounter++;
                X = Read((High << 8) | Low);
                flag_Zero = X == 0;
                flag_Negative = X > 127;
                cycle = 0;
                break;
            }
            case 0xA9:  //LDA Immediate
                A = Read(ProgramCounter);
                flag_Zero = A == 0;
                flag_Negative = A > 127;
                ProgramCounter++;
                cycle = 0;
                break;
            case 0xA5: { //LDA Zero Page
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                A = Read(Temp);
                flag_Zero = A == 0;
                flag_Negative = A > 127;
                cycle = 0;
                break;
            }
            case 0xAD: { //LDA Absolute
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                ProgramCounter++;
                A = Read((High << 8) | Low);
                flag_Zero = A == 0;
                flag_Negative = A > 127;
                cycle = 0;
                break;
            }
            case 0x85: {    //STA Zero Page
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                Write(Temp, A);
                cycle = 0;
                break;
            }
            case 0x8D: //STA Absolute
                {
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                ProgramCounter++;
                Write((High << 8) | Low, A);
                cycle = 0;
                break;
            }
            case 0x86: {    //STX Zero Page
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                Write(Temp, X);
                cycle = 0;
                cycles = 3;
                break;
            }
            case 0x8E: {    //STX Absolute
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                ProgramCounter++;
                Write((High << 8) | Low, X);
                cycle = 0;
                cycles = 4;
                break;
            }
            case 0x84: {    //STY Zero Page
                Temp = Read(ProgramCounter);
                ProgramCounter++;
                Write(Temp, Y);
                cycle = 0;
                cycles = 3;
                break;
            }
            case 0x8C: {    //STY Absolute
                Low = Read(ProgramCounter);
                ProgramCounter++;
                High = Read(ProgramCounter);
                ProgramCounter++;
                Write((High << 8) | Low, Y);
                cycle = 0;
                cycles = 4;
                break;
            }
            case 0xE8:   //INX
                X++;
                flag_Zero = X == 0;
                flag_Negative = X > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0xCA:   //DEX
                X--;
                flag_Zero = X == 0;
                flag_Negative = X > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0xC8:   //INY
                Y++;
                flag_Zero = Y == 0;
                flag_Negative = Y > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0x88:   //DEY
                Y--;
                flag_Zero = Y == 0;
                flag_Negative = Y > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0xAA:  //TAX
                X = A;
                flag_Zero = X == 0;
                flag_Negative = X > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0x8A:  //TXA
                A = X;
                flag_Zero = A == 0;
                flag_Negative = A > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0xA8:  //TAY
                Y = A;
                flag_Zero = Y == 0;
                flag_Negative = Y > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0x98:  //TYA
                A = Y;
                flag_Zero = A == 0;
                flag_Negative = A > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0x9A:  //TXS
                stackPointer = X;
                cycle = 0;
                cycles = 2;
                break;
            case 0xBA:  //TSX
                X = stackPointer;
                flag_Zero = X == 0;
                flag_Negative = X > 127;
                cycle = 0;
                cycles = 2;
                break;
            case 0x38: //SEC
                flag_Carry = true;
                cycle = 0;
                cycles = 2;
                break;
            case 0x18:  //CLC
                flag_Carry = false;
                cycle = 0;
                cycles = 2;
                break;
            case 0xB8:  //CLV
                flag_Overflow = false;
                cycle = 0;
                cycles = 2;
                break;
            case 0x78:  //SEI
                flag_InterruptDisable = true;
                cycle = 0;
                cycles = 2;
                break;
            case 0x58:  //CLI
                flag_InterruptDisable = false;
                cycle = 0;
                cycles = 2;
                break;
            case 0xF8:  //SED
                flag_Decimal = true;
                cycle = 0;
                cycles = 2;
                break;
            case 0xD8:  //CLD
                flag_Decimal = false;
                cycle = 0;
                cycles = 2;
                break;
            case 0xEA:  //NOP
                cycle = 0;
                cycles = 2;
                break;
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