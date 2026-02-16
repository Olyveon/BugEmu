#include <iostream>
#include <cstdint>
#include <array>

uint16_t ProgramCounter;
uint8_t A;
uint8_t X;
uint8_t Y;

std::array<uint8_t, 0x800> RAM{};
std::array<uint8_t, 0x8000> ROM{};

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


int main() {

    return 0;
}