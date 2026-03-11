//
// Created by felipe on 8/03/26.
//

#include "bugCpu.h"

bugCpu::bugCpu() {
	using a = bugCpu;
    Opcode opcodes[256] = {
        { "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
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

uint8_t bugCpu::getFlag(FLAGS flag) {
	return (status & flag);
}

void bugCpu::setFlag(FLAGS flag, bool value) {
	if (value) {
		status |= flag;
	}
	else {
		status &= ~flag;
	}
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

uint8_t bugCpu::ADC() {
	fetch();
	uint16_t r = (uint16_t)A + (uint16_t)fetched + (uint16_t)getFlag(C);
	setFlag(C, r > 255);
	setFlag(Z, (r & 0x00FF) == 0);
	setFlag(N, r & 0x80);
	setFlag(O, ((r ^ (uint16_t)fetched) & (r ^ A) & 0x80));
	A = (uint8_t)r;
	return 1;
}

uint8_t bugCpu::AND() {
	fetch();
	A &= fetched;
	setFlag(Z, A == 0);
	setFlag(N, A > 127);
	return 1;
}

uint8_t bugCpu::ASL() {
	fetch();
	setFlag(C, fetched & 0x80);
	fetched = fetched << 1;
	setFlag(Z, fetched == 0);
	setFlag(N, fetched & 0x80);
	if (opcode == 0x0A)
		A = fetched;
	else
		Write(addr_abs, fetched);
	return 0;
}

uint8_t bugCpu::BCC() {}
uint8_t bugCpu::BCS() {}
uint8_t bugCpu::BEQ() {}
uint8_t bugCpu::BIT() {}
uint8_t bugCpu::BMI() {}
uint8_t bugCpu::BNE() {}
uint8_t bugCpu::BPL() {}
uint8_t bugCpu::BRK() {}
uint8_t bugCpu::BVC() {}
uint8_t bugCpu::BVS() {}
uint8_t bugCpu::CLC() {}
uint8_t bugCpu::CLD() {}
uint8_t bugCpu::CLI() {}
uint8_t bugCpu::CLV() {}
uint8_t bugCpu::CMP() {}
uint8_t bugCpu::CPX() {}
uint8_t bugCpu::CPY() {}
uint8_t bugCpu::DEC() {}
uint8_t bugCpu::DEX() {}
uint8_t bugCpu::DEY() {}
uint8_t bugCpu::EOR() {}
uint8_t bugCpu::INC() {}
uint8_t bugCpu::INX() {}
uint8_t bugCpu::INY() {}
uint8_t bugCpu::JMP() {}
uint8_t bugCpu::JSR() {}
uint8_t bugCpu::LDA() {}
uint8_t bugCpu::LDX() {}
uint8_t bugCpu::LDY() {}
uint8_t bugCpu::LSR() {}
uint8_t bugCpu::NOP() {}
uint8_t bugCpu::ORA() {}
uint8_t bugCpu::PHA() {}
uint8_t bugCpu::PHP() {}
uint8_t bugCpu::PLA() {}
uint8_t bugCpu::PLP() {}
uint8_t bugCpu::ROL() {}
uint8_t bugCpu::ROR() {}
uint8_t bugCpu::RTI() {}
uint8_t bugCpu::RTS() {}
uint8_t bugCpu::SBC() {}
uint8_t bugCpu::SEC() {}
uint8_t bugCpu::SED() {}
uint8_t bugCpu::SEI() {}
uint8_t bugCpu::STA() {}
uint8_t bugCpu::STX() {}
uint8_t bugCpu::STY() {}
uint8_t bugCpu::TAX() {}
uint8_t bugCpu::TAY() {}
uint8_t bugCpu::TSX() {}
uint8_t bugCpu::TXA() {}
uint8_t bugCpu::TXS() {}
uint8_t bugCpu::TYA() {}
uint8_t bugCpu::XXX() {}