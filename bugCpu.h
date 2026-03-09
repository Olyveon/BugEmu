//
// Created by Olyveon on 8/03/26.
//
#include <cstdint>
#include <array>


#ifndef BUGEMU_BUGCPU_H
#define BUGEMU_BUGCPU_H


class bugCpu
{
public:
    bugCpu();
    ~bugCpu();

    //Devices connected to the CPU
    std::array<uint8_t, 0x800> RAM{};
    std::array<uint8_t, 0x8000> ROM{};

    // Read and Write
    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t value);

    // Push and Pull from stack
    void Push(uint8_t value);
    uint8_t Pull();

    enum FLAGS {
        C = (1 << 0),   //Carry Flag
        Z = (1 << 1),   //Zero Flag
        I = (1 << 2),   //Interrupt Disable
        D = (1 << 3),   //Decimal Mode (not really used)
        B = (1 << 4),   //Break Flag
        U = (1 << 5),   //Unused Flag (signed mode)
        O = (1 << 6),   //Overflow
        N = (1 << 7),   //Negative Flag
    };

    bool CPU_Halted = false;

    //System Variables
    uint16_t PC;            //Program Counter
    uint8_t A;              // Accumulator
    uint8_t X;              // X Register
    uint8_t Y;              // Y Register
    uint8_t stackPointer;   //Stack Pointer
    uint8_t opcode;         //Operation Code

    void Emulate_CPU();

private:
    // For easier access for flags
    uint8_t GetFlag(FLAGS flag);
    void    setFlag(FLAGS flag, bool value);
};


#endif //BUGEMU_BUGCPU_H