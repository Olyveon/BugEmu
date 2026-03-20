//
// Created by felipe on 8/03/26.
//

#include "bugCpu.h"

#include <cstring>
#include <format>
#include <stdexcept>
#include <fstream>

// Credit to OLC (One Lone Coder and his wife for putting this table together, no wonder it took them so long to do that)
// I modified it slightly for disassembly purposes
bugCpu::Opcode bugCpu::opcodes[256] = {
        { "BRK", &bugCpu::BRK, &bugCpu::IMM, Imm, 7 },{ "ORA", &bugCpu::ORA, &bugCpu::IZX, IzX, 6 },{ "HLT", &bugCpu::HLT, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 3 },{ "ORA", &bugCpu::ORA, &bugCpu::ZP0, Zp0, 3 },{ "ASL", &bugCpu::ASL, &bugCpu::ZP0, Zp0, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "PHP", &bugCpu::PHP, &bugCpu::IMP, Imp, 3 },{ "ORA", &bugCpu::ORA, &bugCpu::IMM, Imm, 2 },{ "ASL", &bugCpu::ASL, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "ORA", &bugCpu::ORA, &bugCpu::ABS, Abs, 4 },{ "ASL", &bugCpu::ASL, &bugCpu::ABS, Abs, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },
		{ "BPL", &bugCpu::BPL, &bugCpu::REL, Rel, 2 },{ "ORA", &bugCpu::ORA, &bugCpu::IZY, IzY, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "ORA", &bugCpu::ORA, &bugCpu::ZPX, ZpX, 4 },{ "ASL", &bugCpu::ASL, &bugCpu::ZPX, ZpX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "CLC", &bugCpu::CLC, &bugCpu::IMP, Imp, 2 },{ "ORA", &bugCpu::ORA, &bugCpu::ABY, AbY, 4 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "ORA", &bugCpu::ORA, &bugCpu::ABX, AbX, 4 },{ "ASL", &bugCpu::ASL, &bugCpu::ABX, AbX, 7 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },
		{ "JSR", &bugCpu::JSR, &bugCpu::ABS, Abs, 6 },{ "AND", &bugCpu::AND, &bugCpu::IZX, IzX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "BIT", &bugCpu::BIT, &bugCpu::ZP0, Zp0, 3 },{ "AND", &bugCpu::AND, &bugCpu::ZP0, Zp0, 3 },{ "ROL", &bugCpu::ROL, &bugCpu::ZP0, Zp0, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "PLP", &bugCpu::PLP, &bugCpu::IMP, Imp, 4 },{ "AND", &bugCpu::AND, &bugCpu::IMM, Imm, 2 },{ "ROL", &bugCpu::ROL, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "BIT", &bugCpu::BIT, &bugCpu::ABS, Abs, 4 },{ "AND", &bugCpu::AND, &bugCpu::ABS, Abs, 4 },{ "ROL", &bugCpu::ROL, &bugCpu::ABS, Abs, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },
		{ "BMI", &bugCpu::BMI, &bugCpu::REL, Rel, 2 },{ "AND", &bugCpu::AND, &bugCpu::IZY, IzY, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "AND", &bugCpu::AND, &bugCpu::ZPX, ZpX, 4 },{ "ROL", &bugCpu::ROL, &bugCpu::ZPX, ZpX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "SEC", &bugCpu::SEC, &bugCpu::IMP, Imp, 2 },{ "AND", &bugCpu::AND, &bugCpu::ABY, AbY, 4 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "AND", &bugCpu::AND, &bugCpu::ABX, AbX, 4 },{ "ROL", &bugCpu::ROL, &bugCpu::ABX, AbX, 7 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },
		{ "RTI", &bugCpu::RTI, &bugCpu::IMP, Imp, 6 },{ "EOR", &bugCpu::EOR, &bugCpu::IZX, IzX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 3 },{ "EOR", &bugCpu::EOR, &bugCpu::ZP0, Zp0, 3 },{ "LSR", &bugCpu::LSR, &bugCpu::ZP0, Zp0, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "PHA", &bugCpu::PHA, &bugCpu::IMP, Imp, 3 },{ "EOR", &bugCpu::EOR, &bugCpu::IMM, Imm, 2 },{ "LSR", &bugCpu::LSR, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "JMP", &bugCpu::JMP, &bugCpu::ABS, Abs, 3 },{ "EOR", &bugCpu::EOR, &bugCpu::ABS, Abs, 4 },{ "LSR", &bugCpu::LSR, &bugCpu::ABS, Abs, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },
		{ "BVC", &bugCpu::BVC, &bugCpu::REL, Rel, 2 },{ "EOR", &bugCpu::EOR, &bugCpu::IZY, IzY, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "EOR", &bugCpu::EOR, &bugCpu::ZPX, ZpX, 4 },{ "LSR", &bugCpu::LSR, &bugCpu::ZPX, ZpX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "CLI", &bugCpu::CLI, &bugCpu::IMP, Imp, 2 },{ "EOR", &bugCpu::EOR, &bugCpu::ABY, AbY, 4 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "EOR", &bugCpu::EOR, &bugCpu::ABX, AbX, 4 },{ "LSR", &bugCpu::LSR, &bugCpu::ABX, AbX, 7 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },
		{ "RTS", &bugCpu::RTS, &bugCpu::IMP, Imp, 6 },{ "ADC", &bugCpu::ADC, &bugCpu::IZX, IzX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 3 },{ "ADC", &bugCpu::ADC, &bugCpu::ZP0, Zp0, 3 },{ "ROR", &bugCpu::ROR, &bugCpu::ZP0, Zp0, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "PLA", &bugCpu::PLA, &bugCpu::IMP, Imp, 4 },{ "ADC", &bugCpu::ADC, &bugCpu::IMM, Imm, 2 },{ "ROR", &bugCpu::ROR, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "JMP", &bugCpu::JMP, &bugCpu::IND, Ind, 5 },{ "ADC", &bugCpu::ADC, &bugCpu::ABS, Abs, 4 },{ "ROR", &bugCpu::ROR, &bugCpu::ABS, Abs, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },
		{ "BVS", &bugCpu::BVS, &bugCpu::REL, Rel, 2 },{ "ADC", &bugCpu::ADC, &bugCpu::IZY, IzY, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "ADC", &bugCpu::ADC, &bugCpu::ZPX, ZpX, 4 },{ "ROR", &bugCpu::ROR, &bugCpu::ZPX, ZpX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "SEI", &bugCpu::SEI, &bugCpu::IMP, Imp, 2 },{ "ADC", &bugCpu::ADC, &bugCpu::ABY, AbY, 4 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "ADC", &bugCpu::ADC, &bugCpu::ABX, AbX, 4 },{ "ROR", &bugCpu::ROR, &bugCpu::ABX, AbX, 7 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },
		{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "STA", &bugCpu::STA, &bugCpu::IZX, IzX, 6 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "STY", &bugCpu::STY, &bugCpu::ZP0, Zp0, 3 },{ "STA", &bugCpu::STA, &bugCpu::ZP0, Zp0, 3 },{ "STX", &bugCpu::STX, &bugCpu::ZP0, Zp0, 3 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 3 },{ "DEY", &bugCpu::DEY, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "TXA", &bugCpu::TXA, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "STY", &bugCpu::STY, &bugCpu::ABS, Abs, 4 },{ "STA", &bugCpu::STA, &bugCpu::ABS, Abs, 4 },{ "STX", &bugCpu::STX, &bugCpu::ABS, Abs, 4 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 4 },
		{ "BCC", &bugCpu::BCC, &bugCpu::REL, Rel, 2 },{ "STA", &bugCpu::STA, &bugCpu::IZY, IzY, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "STY", &bugCpu::STY, &bugCpu::ZPX, ZpX, 4 },{ "STA", &bugCpu::STA, &bugCpu::ZPX, ZpX, 4 },{ "STX", &bugCpu::STX, &bugCpu::ZPY, ZpY, 4 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 4 },{ "TYA", &bugCpu::TYA, &bugCpu::IMP, Imp, 2 },{ "STA", &bugCpu::STA, &bugCpu::ABY, AbY, 5 },{ "TXS", &bugCpu::TXS, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 5 },{ "STA", &bugCpu::STA, &bugCpu::ABX, AbX, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },
		{ "LDY", &bugCpu::LDY, &bugCpu::IMM, Imm, 2 },{ "LDA", &bugCpu::LDA, &bugCpu::IZX, IzX, 6 },{ "LDX", &bugCpu::LDX, &bugCpu::IMM, Imm, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "LDY", &bugCpu::LDY, &bugCpu::ZP0, Zp0, 3 },{ "LDA", &bugCpu::LDA, &bugCpu::ZP0, Zp0, 3 },{ "LDX", &bugCpu::LDX, &bugCpu::ZP0, Zp0, 3 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 3 },{ "TAY", &bugCpu::TAY, &bugCpu::IMP, Imp, 2 },{ "LDA", &bugCpu::LDA, &bugCpu::IMM, Imm, 2 },{ "TAX", &bugCpu::TAX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "LDY", &bugCpu::LDY, &bugCpu::ABS, Abs, 4 },{ "LDA", &bugCpu::LDA, &bugCpu::ABS, Abs, 4 },{ "LDX", &bugCpu::LDX, &bugCpu::ABS, Abs, 4 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 4 },
		{ "BCS", &bugCpu::BCS, &bugCpu::REL, Rel, 2 },{ "LDA", &bugCpu::LDA, &bugCpu::IZY, IzY, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "LDY", &bugCpu::LDY, &bugCpu::ZPX, ZpX, 4 },{ "LDA", &bugCpu::LDA, &bugCpu::ZPX, ZpX, 4 },{ "LDX", &bugCpu::LDX, &bugCpu::ZPY, ZpY, 4 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 4 },{ "CLV", &bugCpu::CLV, &bugCpu::IMP, Imp, 2 },{ "LDA", &bugCpu::LDA, &bugCpu::ABY, AbY, 4 },{ "TSX", &bugCpu::TSX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 4 },{ "LDY", &bugCpu::LDY, &bugCpu::ABX, AbX, 4 },{ "LDA", &bugCpu::LDA, &bugCpu::ABX, AbX, 4 },{ "LDX", &bugCpu::LDX, &bugCpu::ABY, AbY, 4 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 4 },
		{ "CPY", &bugCpu::CPY, &bugCpu::IMM, Imm, 2 },{ "CMP", &bugCpu::CMP, &bugCpu::IZX, IzX, 6 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "CPY", &bugCpu::CPY, &bugCpu::ZP0, Zp0, 3 },{ "CMP", &bugCpu::CMP, &bugCpu::ZP0, Zp0, 3 },{ "DEC", &bugCpu::DEC, &bugCpu::ZP0, Zp0, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "INY", &bugCpu::INY, &bugCpu::IMP, Imp, 2 },{ "CMP", &bugCpu::CMP, &bugCpu::IMM, Imm, 2 },{ "DEX", &bugCpu::DEX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "CPY", &bugCpu::CPY, &bugCpu::ABS, Abs, 4 },{ "CMP", &bugCpu::CMP, &bugCpu::ABS, Abs, 4 },{ "DEC", &bugCpu::DEC, &bugCpu::ABS, Abs, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },
		{ "BNE", &bugCpu::BNE, &bugCpu::REL, Rel, 2 },{ "CMP", &bugCpu::CMP, &bugCpu::IZY, IzY, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "CMP", &bugCpu::CMP, &bugCpu::ZPX, ZpX, 4 },{ "DEC", &bugCpu::DEC, &bugCpu::ZPX, ZpX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "CLD", &bugCpu::CLD, &bugCpu::IMP, Imp, 2 },{ "CMP", &bugCpu::CMP, &bugCpu::ABY, AbY, 4 },{ "NOP", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "CMP", &bugCpu::CMP, &bugCpu::ABX, AbX, 4 },{ "DEC", &bugCpu::DEC, &bugCpu::ABX, AbX, 7 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },
		{ "CPX", &bugCpu::CPX, &bugCpu::IMM, Imm, 2 },{ "SBC", &bugCpu::SBC, &bugCpu::IZX, IzX, 6 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "CPX", &bugCpu::CPX, &bugCpu::ZP0, Zp0, 3 },{ "SBC", &bugCpu::SBC, &bugCpu::ZP0, Zp0, 3 },{ "INC", &bugCpu::INC, &bugCpu::ZP0, Zp0, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 5 },{ "INX", &bugCpu::INX, &bugCpu::IMP, Imp, 2 },{ "SBC", &bugCpu::SBC, &bugCpu::IMM, Imm, 2 },{ "NOP", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::SBC, &bugCpu::IMP, Imp, 2 },{ "CPX", &bugCpu::CPX, &bugCpu::ABS, Abs, 4 },{ "SBC", &bugCpu::SBC, &bugCpu::ABS, Abs, 4 },{ "INC", &bugCpu::INC, &bugCpu::ABS, Abs, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },
		{ "BEQ", &bugCpu::BEQ, &bugCpu::REL, Rel, 2 },{ "SBC", &bugCpu::SBC, &bugCpu::IZY, IzY, 5 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 8 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "SBC", &bugCpu::SBC, &bugCpu::ZPX, ZpX, 4 },{ "INC", &bugCpu::INC, &bugCpu::ZPX, ZpX, 6 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 6 },{ "SED", &bugCpu::SED, &bugCpu::IMP, Imp, 2 },{ "SBC", &bugCpu::SBC, &bugCpu::ABY, AbY, 4 },{ "NOP", &bugCpu::NOP, &bugCpu::IMP, Imp, 2 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },{ "???", &bugCpu::NOP, &bugCpu::IMP, Imp, 4 },{ "SBC", &bugCpu::SBC, &bugCpu::ABX, AbX, 4 },{ "INC", &bugCpu::INC, &bugCpu::ABX, AbX, 7 },{ "???", &bugCpu::XXX, &bugCpu::IMP, Imp, 7 },
    };

bugCpu::bugCpu() = default;

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
	clockCount++;
	if (cycles == 0) {
		opcode = Read(PC);
		instPC = PC;
		PC++;
		// We save the registers and flags from BEFORE the instruction is executed
		if (logging) {
			log_a = A;
			log_x = X;
			log_y = Y;
			log_sp = stackPointer;
			log_status = status;
		}

		cycles = opcodes[opcode].cycles;

		uint8_t add1 = (this->*opcodes[opcode].addr)();
		uint8_t add2 = (this->*opcodes[opcode].op)();


		cycles += (add1 & add2);
		// Logging
		if (logging) {
			traceEntry entry;
			entry.disassembly = getDisassembly();
			entry.registers = parseRegisters(log_a, log_x, log_y, log_sp);
			entry.flags = parseFlags(log_status);
			entry.cycles = clockCount;
			if (traceLog.size() >= MAX_TRACE_LENGTH)
				traceLog.pop_front();
			traceLog.push_back(entry);
		}
	}
	cycles--;
}

// Disassembles the opcode based on its addressing mode and the instruction it calls
//
std::string bugCpu::getDisassembly() {
	std::string disassem_addr;
	switch (opcodes[opcode].mode) {
		case Imp:
			disassem_addr = " ";
			break;
		case Imm:
			disassem_addr = std::format("#{:02X}", fetched);	// Technically the convention would be to put a $ before the value, but it honestly looks bad and that's the only reason I won't use it :)
			break;
		case Abs:
			disassem_addr = std::format("${:04X}", addr_abs);
			break;
		case Zp0:
			disassem_addr = std::format("${:02X}", Read(instPC + 1));
			break;
		case Rel:
			disassem_addr = std::format("${:04X}", addr_abs);
			break;
		case ZpX:
			disassem_addr = std::format("${:02X}, X [${:04X}] = ${:02X}", Read(instPC + 1), addr_abs, Read(addr_abs));
			break;
		case ZpY:
			disassem_addr = std::format("${:02X}, Y [${:04X}] = ${:02X}", Read(instPC + 1), addr_abs, Read(addr_abs));
			break;
		case AbX:
			disassem_addr = std::format("${:04X}, X", addr_abs);
			break;
		case AbY:
			disassem_addr = std::format("${:04X}, Y", addr_abs);
			break;
		case Ind:
			disassem_addr = std::format("(${:04X}) = ${:04X}", ind_addr, addr_abs);
			break;
		case IzX:
			disassem_addr = std::format("(${:04X}, X) = ${:04X}", Read(instPC+1), addr_abs);
			break;
		case IzY:
			disassem_addr = std::format("(${:04X}), Y = ${:04X}", Read(instPC+1), addr_abs);
			break;


	}
	std::string disassembly = std::format("{:X}: {:02X} {} {}",instPC, opcode, opcodes[opcode].name, disassem_addr);
	return disassembly;
}

std::string bugCpu::parseRegisters(uint8_t a, uint8_t x, uint8_t y, uint8_t sp) {
	std::string registers = std::format("A: 0x{:02X}, X: 0x{:02X}, Y: 0x{:02X}, SP: 0x{:02X}", a, x, y, sp);
	return registers;
}

std::string bugCpu::parseFlags(uint8_t stat) {
	std::string flags;
	flags.push_back(getFlag(N, stat) ? 'N' : 'n');
	flags.push_back(getFlag(V, stat) ? 'V' : 'v');
	flags.push_back('-');
	flags.push_back('-');
	flags.push_back(getFlag(D, stat) ? 'D' : 'd');
	flags.push_back(getFlag(I, stat) ? 'I' : 'i');
	flags.push_back(getFlag(Z, stat) ? 'Z' : 'z');
	flags.push_back(getFlag(C, stat) ? 'C' : 'c');
	return flags;
}

void bugCpu::run() {
	while (!CPU_Halted) {
		clock();
	}
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
	setFlag(U, 1);
	stackPointer = 0xFD;
	clockCount += 7;	// it takes 7 cycles for these operations to finish
}

uint8_t bugCpu::getFlag(FLAGS flag) {
	return ((status & flag) > 0) ? 1 : 0;
}

uint8_t bugCpu::getFlag(FLAGS flag, uint8_t flags) {
	return ((flags & flag) > 0) ? 1 : 0;
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
	addr_abs = Read(PC);
	PC++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t bugCpu::ZPX() {
	addr_abs = (Read(PC) + X);
	PC++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t bugCpu::ZPY() {
	addr_abs = (Read(PC) + Y);
	PC++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t bugCpu::REL() {
	addr_rel = Read(PC++);
	if (addr_rel & 0x80) {	//Has negative bit
		addr_rel |= 0xFF00;		// Since addr_rel is a 16 bit value we need to make it a negative on 16 bits aswell
	}
	return 0;
}

uint8_t bugCpu::IND() {
	uint16_t Low = Read(PC++);
	uint16_t High = Read(PC++);
	uint16_t Temp = (High << 8) | Low;
	if (logging) ind_addr = Temp;
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
	if (!(opcodes[opcode].addr == &bugCpu::IMP))
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
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch on Carry set
uint8_t bugCpu::BCS() {
	if (getFlag(C) == 1) {
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch if Equal (Z == 1)
uint8_t bugCpu::BEQ() {
	if (getFlag(Z) == 1) {
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
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
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch if Not Equal (Z == 0)
uint8_t bugCpu::BNE() {
	if (getFlag(Z) == 0) {
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch on Plus (N == 0)
uint8_t bugCpu::BPL() {
	if (getFlag(N) == 0) {
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
		cycles++;
		return 1;
	}
	return 0;
}

// Break
uint8_t bugCpu::BRK() {
	Push(uint8_t(PC >> 8));	// Pushes High Byte
	Push(uint8_t(PC & 0xFF));	// Pushes Low Byte
	uint8_t Temp = status | 0x10;	// Sets bit 4 (Break flag) before pushing to stack
	Push(Temp);
	setFlag(I, true);
	uint8_t TLow = Read(0xFFFE);
	uint8_t THigh = Read(0xFFFF);
	PC = (THigh << 8) | TLow;	// Jumps to IRQ Handler
	return 0;
}

// Branch on Overflow Clear
uint8_t bugCpu::BVC() {
	if (getFlag(V) == 0) {
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
		cycles++;
		return 1;
	}
	return 0;
}

// Branch on Overflow Set
uint8_t bugCpu::BVS() {
	if (getFlag(V) == 1) {
		addr_abs = PC + addr_rel;
		if ((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		PC = addr_abs;
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
	return 0;
}