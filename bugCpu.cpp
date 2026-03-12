//
// Created by felipe on 8/03/26.
//

#include "bugCpu.h"

#include <cstring>
#include <stdexcept>
#include <fstream>

bugCpu::bugCpu() {
	using a = bugCpu;
    Opcode opcodes[256] = {
        { "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "HLT", &a::HLT, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
    };
}

bugCpu::~bugCpu()= default;

uint8_t bugCpu::Read(uint16_t address) {
	if (address < 0x800) {
		return RAM[address];
	}
	if (address <= 0x1FFF) {
		constexpr uint16_t MASK = 0x7FF;
		return RAM[address & MASK];
	}
	if (address >= 0x8000) {
		return ROM[address - 0x8000];
	}
	return 0;
}

void bugCpu::Write(uint16_t address, uint8_t value) {
	if (address < 0x800) {
		RAM[address] = value;
	}
	if (address <= 0x1FFF) {
		constexpr uint16_t MASK = 0x7FF;
		RAM[address & MASK] = value;
	}
}

void bugCpu::clock() {
	if (cycles == 0) {
		opcode = Read(PC);
		PC++;

		cycles = opcodes[opcode].cycles;

		uint8_t add1 = (this->*opcodes[opcode].addr)();
		uint8_t add2 = (this->*opcodes[opcode].op)();

		cycles += (add1 & add2);
	}
	cycles--;
}

std::vector<uint8_t> bugCpu::ReadAllBytes(const std::string& path)
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

void bugCpu::reset() {
	auto HeaderedROM = ReadAllBytes(filepath);
	std::memcpy(ROM.data(), HeaderedROM.data() + 0x10, 0x8000);
	std::memcpy(Header.data(), HeaderedROM.data() + 0x10, 0x10);
	uint8_t PCL = Read(0xFFFC);
	uint8_t PCH = Read(0xFFFD);
	PC = (PCH << 8) | PCL;
	setFlag(I, 1);
	stackPointer = 0xFD;
	clock();
}

uint8_t bugCpu::getFlag(FLAGS flag) {
	return ((status & flag) > 0) ? 1 : 0;
}

void bugCpu::setFlag(FLAGS flag, bool value) {
	if (value) {
		status |= flag;
	}
	else {
		status &= ~flag;
	}
}

void bugCpu::Push(uint8_t value) {
	Write((0x100 | stackPointer), value);
	stackPointer --;
}

uint8_t bugCpu::Pull() {
	stackPointer++;
	uint8_t Temp = Read((0x100 | stackPointer));
	return Temp;
}

// Addressing Modes
uint8_t bugCpu::IMP() {
	fetched = A;
	return 0;
}

uint8_t bugCpu::ABS() {
	uint8_t Low = Read(PC++);
	uint8_t High = Read(PC++);
	addr_abs = (High << 8) | Low;
	return 0;
}

uint8_t bugCpu::ABX() {
	uint8_t Low = Read(PC++);
	uint8_t High = Read(PC++);
	addr_abs = (High << 8) | Low;
	addr_abs += X;

	if ((addr_abs & 0xFF00) != (High << 8))
		return 1;
	else
		return 0;
}

uint8_t bugCpu::ABY() {
	uint8_t Low = Read(PC++);
	uint8_t High = Read(PC++);
	addr_abs = (High << 8) | Low;
	addr_abs += Y;

	if ((addr_abs & 0xFF00) != (High << 8))
		return 1;
	else
		return 0;
}

uint8_t bugCpu::IMM() {
	addr_abs = PC++;
	return 0;
}

uint8_t bugCpu::ZP0() {
	addr_abs = Read(PC++);
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t bugCpu::ZPX() {
	addr_abs = (Read(PC) + X);
	addr_abs &= 0x00FF;
	PC++;
	return 0;
}

uint8_t bugCpu::ZPY() {
	addr_abs = (Read(PC) + Y);
	PC++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t bugCpu::REL() {
	uint8_t Temp = Read(PC++);
	if (Temp > 127) {	//Has negative bit
		Temp -= 256;
	}
	addr_rel = Temp;
	return 0;
}

uint8_t bugCpu::IND() {
	uint16_t Low = Read(PC++);
	uint16_t High = Read(PC++);
	uint16_t Temp = (High << 8) | Low;
	if (Low == 0xFF) {	// Page will be crossed
		uint8_t TLow = Read(Temp);
		uint8_t THigh = Read(High << 8);
		addr_abs = (THigh << 8) | TLow;
		return 1;
	} else {
		uint8_t TLow = Read(Temp);
		uint8_t THigh = Read(Temp + 1);
		addr_abs = (THigh << 8) | TLow;
		return 0;
	}
}

uint8_t bugCpu::IZX() {
	uint16_t temp = Read(PC++);

	uint16_t Low = Read(static_cast<uint16_t>(temp + X) & 0x00FF);
	uint16_t High = Read(static_cast<uint16_t>(temp + X + 1) & 0x00FF);

	addr_abs = (High << 8) | Low;
	return 0;
}

uint8_t bugCpu::IZY() {
	uint16_t temp = Read(PC++);

	uint16_t Low = Read(temp & 0x00FF);
	uint16_t High = Read((temp + 1) & 0x00FF);

	addr_abs = (High << 8) | Low;
	addr_abs += Y;
	if ((addr_abs & 0xFF00) != (High << 8))  //Page Crossed
		return 1;
	return 0;
}

uint8_t bugCpu::fetch() {
	if (!(opcodes[opcode].addr == &bugCpu::IMM))
		fetched = Read(addr_abs);
	return fetched;
}

//Add on Carry
uint8_t bugCpu::ADC() {
	fetch();
	uint16_t r = (uint16_t)A + (uint16_t)fetched + (uint16_t)getFlag(C);
	setFlag(C, r > 255);
	setFlag(Z, (r & 0x00FF) == 0);
	setFlag(N, r & 0x80);
	setFlag(V, ((r ^ (uint16_t)fetched) & (r ^ A) & 0x80));
	A = (uint8_t)r;
	return 1;
}

//Bitwise AND between data and accumulator
uint8_t bugCpu::AND() {
	fetch();
	A &= fetched;
	setFlag(Z, A == 0);
	setFlag(N, A > 127);
	return 1;
}

// Arithmetic Shift Left
uint8_t bugCpu::ASL() {
	fetch();
	setFlag(C, fetched & 0x80);
	fetched = fetched << 1;
	setFlag(Z, fetched == 0);
	setFlag(N, fetched & 0x80);
	if (opcode == 0x0A)	// If implied
		A = fetched;
	else
		Write(addr_abs, fetched);
	return 0;
}

// Branch on Carry clear
uint8_t bugCpu::BCC() {
	if (getFlag(C) == 0) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch on Carry set
uint8_t bugCpu::BCS() {
	if (getFlag(C) == 1) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch if Equal (Z == 1)
uint8_t bugCpu::BEQ() {
	if (getFlag(Z) == 1) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Bit test
uint8_t bugCpu::BIT() {
	fetch();
	uint8_t Temp = A & fetched;
	setFlag(Z, Temp == 0);
	setFlag(N, fetched & 0x80);
	setFlag(V, fetched & 0x40);
	return 0;
}

// Branch if minus
uint8_t bugCpu::BMI() {
	if (getFlag(N) == 1) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch if Not Equal (Z == 0)
uint8_t bugCpu::BNE() {
	if (getFlag(Z) == 0) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch on Plus (N == 0)
uint8_t bugCpu::BPL() {
	if (getFlag(N) == 0) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Break
uint8_t bugCpu::BRK() {
	PC++;
	Push(uint8_t(PC >> 8));	// Pushes High Byte
	Push(uint8_t(PC & 0xFF));	// Pushes Low Byte
	uint8_t Temp = status | 0x10;	// Sets bit 4 (Break flag) before pushing to stack
	Push(Temp);
	setFlag(I, true);
	uint8_t THigh = Read(0xFFFF);
	uint8_t TLow = Read(0xFFFE);
	PC = (THigh << 8) | TLow;	// Jumps to IRQ Handler
	return 0;
}

// Branch on Overflow Clear
uint8_t bugCpu::BVC() {
	if (getFlag(V) == 0) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch on Overflow Set
uint8_t bugCpu::BVS() {
	if (getFlag(V) == 1) {
		PC = PC + addr_rel;
		cycles++;
		return 1;
	}
	return 0;
}

// Clear Carry
uint8_t bugCpu::CLC() {
	setFlag(C, false);
	return 0;
}

// Clear Decimal
uint8_t bugCpu::CLD() {
	setFlag(D, false);
	return 0;
}

// Clear Interrupt-Disable
uint8_t bugCpu::CLI() {
	setFlag(I, false);
	return 0;
}

// Clear Overflow
uint8_t bugCpu::CLV() {
	setFlag(V, false);
	return 0;
}

/*	Compare
 	It compares the value on memory with the accumulator, it does so with a substraction
 */
uint8_t bugCpu::CMP() {
	fetch();
	uint8_t Temp = A - fetched;		// Yes, I know this is technically innecessary but the 6502 does a substraction so, so do i :p (don't ask me about accuracy on PPU cycles)
	setFlag(Z, Temp == 0);
	setFlag(N, Temp & 0x80);
	setFlag(C, A >= fetched);	// If A is greater(or equal) it means that it won't have to borrow bits, AKA when doing the substraction it won't have to "wrap" or overflow, example: A=0x03 memory=0x05, A - memory = -2 (signed 0xFE) thus it has to wrap, or borrow bits, thus we need to clear carry.
	return 1;
}

// Compare X
uint8_t bugCpu::CPX() {
	fetch();
	uint8_t Temp = X - fetched;
	setFlag(Z, Temp == 0);
	setFlag(N, Temp & 0x80);
	setFlag(C, X >= fetched);
	return 0;
}

// Compare Y
uint8_t bugCpu::CPY() {
	fetch();
	uint8_t Temp = Y - fetched;
	setFlag(Z, Temp == 0);
	setFlag(N, Temp & 0x80);
	setFlag(C, Y >= fetched);
	return 0;
}

// Decrement Memory
uint8_t bugCpu::DEC() {
	fetch();
	fetched--;
	setFlag(Z, fetched == 0);
	setFlag(N, fetched & 0x80);
	Write(addr_abs, fetched);
	return 0;
}

// Decrement X
uint8_t bugCpu::DEX() {
	X--;
	setFlag(Z, X == 0);
	setFlag(N, X & 0x80);
	return 0;
}

// Decrement Y
uint8_t bugCpu::DEY() {
	Y--;
	setFlag(Z, Y == 0);
	setFlag(N, Y & 0x80);
	return 0;
}

// Exclusive OR (XOR), it's called EOR because they were too swag for XOR or smt
uint8_t bugCpu::EOR() {
	fetch();
	A = A ^ fetched;
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
	return 1;
}

// Increment memory, same as DEC but INC duh
uint8_t bugCpu::INC() {
	fetch();
	fetched++;
	setFlag(Z, fetched == 0);
	setFlag(N, fetched & 0x80);
	Write(addr_abs, fetched);
	return 0;
}

// Increment X
uint8_t bugCpu::INX() {
	X++;
	setFlag(Z, X == 0);
	setFlag(N, X & 0x80);
	return 0;
}

// Increment Y
uint8_t bugCpu::INY() {
	Y++;
	setFlag(Z, Y == 0);
	setFlag(N, Y & 0x80);
	return 0;
}

// Jump to an address
uint8_t bugCpu::JMP() {
	PC = addr_abs;
	return 0;
}

// Jump to subroutine
uint8_t bugCpu::JSR() {
	PC--;
	Push(PC >> 8);  //Push PCH
	Push(static_cast<uint8_t>(PC)); //Push PCL
	PC = addr_abs;
	return 0;
}

// Load memory onto A
uint8_t bugCpu::LDA() {
	fetch();
	A = fetched;
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
	return 1;
}

// Load memory onto X
uint8_t bugCpu::LDX() {
	fetch();
	X = fetched;
	setFlag(Z, X == 0);
	setFlag(N, X & 0x80);
	return 1;
}

// Load memory onto Y
uint8_t bugCpu::LDY() {
	fetch();
	Y = fetched;
	setFlag(Z, Y == 0);
	setFlag(N, Y & 0x80);
	return 1;
}

uint8_t bugCpu::LSR() {
	fetch();
	setFlag(C, fetched & 0x01);
	fetched = fetched >> 1;
	setFlag(Z, fetched == 0);
	setFlag(N, false);
	if (opcode == 0x4A)	// If implied
		A = fetched;
	else
		Write(addr_abs, fetched);
	return 0;
}

uint8_t bugCpu::NOP() {
	return 0;
}

// Bitwise OR with accumulator
uint8_t bugCpu::ORA() {
	fetch();
	A |= fetched;
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
	return 1;
}

// Push A
uint8_t bugCpu::PHA() {
	Push(A);
	return 0;
}

// Push Processor status
uint8_t bugCpu::PHP() {
	uint8_t Temp = status;
	Temp |= 0x10; // Sets break flag before pushing
	Push(Temp);
	return 0;
}

// Pull A
uint8_t bugCpu::PLA() {
	A = Pull();
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
	return 0;
}

// Pull status flags
uint8_t bugCpu::PLP() {
	status = Pull();
	return 0;
}

// Rotate Left
uint8_t bugCpu::ROL() {
	fetch();
	uint8_t Temp = fetched & 0x80;
	fetched = fetched << 1;
	if (getFlag(C)) {
		fetched |= 0x01;
	}
	setFlag(Z, fetched == 0);
	setFlag(N, fetched & 0x80);
	setFlag(C, Temp);
	if (opcode == 0x2A)	// If implied
		A = fetched;
	else
		Write(addr_abs, fetched);
	return 0;
}

// Rotate Right
uint8_t bugCpu::ROR() {
	fetch();
	uint8_t Temp = fetched & 0x01;
	fetched = fetched >> 1;
	if (getFlag(C)) {
		fetched |= 0x80;
	}
	setFlag(Z, fetched == 0);
	setFlag(N, fetched & 0x80);
	setFlag(C, Temp);
	if (opcode == 0x6A)	// If implied
		A = fetched;
	else
		Write(addr_abs, fetched);
	return 0;
}

// Return from Interrupt
uint8_t bugCpu::RTI() {
	status = Pull();
	uint8_t PCL = Pull();
	uint8_t PCH = Pull();
	PC = (PCH << 8) | PCL;
	return 0;
}

// Return from Subroutine
uint8_t bugCpu::RTS() {
	uint8_t PCL = Pull();
	uint8_t PCH = Pull();
	PC = (PCH << 8) | PCL;
	PC++;
	return 0;
}

// Subtract with carry
uint8_t bugCpu::SBC() {
	fetch();
	uint16_t r = (uint16_t)A - (uint16_t)fetched - (1 -(uint16_t)getFlag(C));
	setFlag(C, !(r & 0xFF00));
	setFlag(Z, (r & 0x00FF) == 0);
	setFlag(N, r & 0x80);
	setFlag(V, ((A ^ fetched) & (A ^ r) & 0x80));
	A = (uint8_t)r;
	return 1;
}

// Set carry
uint8_t bugCpu::SEC() {
	setFlag(C, true);
	return 0;
}

// Set Decimal
uint8_t bugCpu::SED() {
	setFlag(D, true);
	return 0;
}

// Set Interrupt-Disable
uint8_t bugCpu::SEI() {
	setFlag(I, true);
	return 0;
}

// Store A
uint8_t bugCpu::STA() {
	Write(addr_abs, A);
	return 0;
}

// Store X
uint8_t bugCpu::STX() {
	Write(addr_abs, X);
	return 0;
}

// Store Y
uint8_t bugCpu::STY() {
	Write(addr_abs, Y);
	return 0;
}

// Transfer A to X
uint8_t bugCpu::TAX() {
	X = A;
	setFlag(Z, X == 0);
	setFlag(N, X & 0x80);
	return 0;
}

// Transfer A to Y
uint8_t bugCpu::TAY() {
	Y = A;
	setFlag(Z, Y == 0);
	setFlag(N, Y & 0x80);
	return 0;
}

// Transfer Stack Pointer to X
uint8_t bugCpu::TSX() {
	X = stackPointer;
	setFlag(Z, X == 0);
	setFlag(N, X & 0x80);
	return 0;
}

// Transfer X to A
uint8_t bugCpu::TXA() {
	A = X;
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
	return 0;
}

// Transfer X to stack pointer
uint8_t bugCpu::TXS() {
	stackPointer = X;
	return 0;
}

// Transfer Y to A
uint8_t bugCpu::TYA() {
	A = Y;
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
	return 0;
}

// Unofficial code HLT or STP
uint8_t bugCpu::HLT() {
	CPU_Halted = true;
	return 0;
}

uint8_t bugCpu::XXX() {

}