//
// Created by felipe on 22/03/26.
//

#include "bugPpu.h"
#include "bugNES.h"
#include <fstream>
#include <iomanip>

bugPpu::bugPpu() :
        // a NTSC color palette
        nesPalette{
            0x666666FF, 0x002A88FF, 0x1412A7FF, 0x3B00A4FF, 0x5C007EFF, 0x6E0040FF, 0x6C0600FF, 0x561D00FF,
            0x333500FF, 0x0B4800FF, 0x005200FF, 0x004F08FF, 0x00404DFF, 0x000000FF, 0x000000FF, 0x000000FF,
            0xADADADFF, 0x155FD9FF, 0x4240FFFF, 0x7527FEFF, 0xA01ACCFF, 0xB71E7BFF, 0xB53120FF, 0x994E00FF,
            0x6B6D00FF, 0x388700FF, 0x0C9300FF, 0x008F32FF, 0x007C8DFF, 0x000000FF, 0x000000FF, 0x000000FF,
            0xFFFEFFFF, 0x64B0FFFF, 0x9291FFFF, 0xC686FFFF, 0xEE7EFFFF, 0xFE7EDCCF, 0xFE90BEFF, 0xF0A62AFF,
            0xCCBC00FF, 0xB3D000FF, 0x8BE111FF, 0x6FE35EFF, 0x61D096FF, 0x4F4F4FFF, 0x000000FF, 0x000000FF,
            0xFFFEFFFF, 0xBE9FFFFF, 0xB8B8FFFF, 0xD8B8FFFF, 0xFFB7FFFF, 0xFFB7E7FF, 0xFFBCC7FF, 0xFFC785FF,
            0xF3D200FF, 0xE2E200FF, 0xB8E834FF, 0xB0EFA0FF, 0xAAECF0FF, 0xB4B4B4FF, 0x000000FF, 0x000000FF
        } {}

bugPpu::~bugPpu() {
}

// This function is for when the PPU needs to read
uint8_t bugPpu::ppuRead(uint16_t address) {
    return nes->ppuRead(address);
}

// This function is for when the PPU needs to write
void bugPpu::ppuWrite(uint16_t address, uint8_t data) {
    nes->ppuWrite(address, data);
}

void bugPpu::reload() {
    cycle = 0;
    scanline = 0;
    v = 0;
    t = 0;
    w = false;
    ppuReadBuffer = 0;
    VRAM = {};
    paletteRAM = {};
    ctrl.value = 0x00;
    mask.value = 0x00;
    status.value = 0x00;
}

// This function is for when the CPU is writing TO the PPU
void bugPpu::cpuWrite(uint16_t address, uint8_t data) {
    switch (address) {
        case 0x2000:    // PPUCTRL
            ctrl.value = data;
            break;
        case 0x2001:    // PPUMASK
            mask.value = data;
            break;
        case 0x2002:    // PPUSTATUS
            break;
        case 0x2003:    // OAMADDR
            break;
        case 0x2004:    // OAMDATA
            break;
        case 0x2005:    // PPUSCROLL
            if (!w) {
                scroll.x = data & 0x07;
                temp = ((temp & 0x7FE0 ) | (uint16_t(data) >> 3));
            }
            else {
                t = (t & 0x801F) | (((data & 0xF8) << 2) | ((data & 7) << 12));
            }
            w = !w;
            break;
        case 0x2006:    // PPUADDR
            if (!w) {
                // the first write goes towards the high byte, we mask it $3F since bits 15 and 14 of VRAM address are ignored
                temp = (data & 0x3F) << 8;
                // we wait until we have the low byte to write the address
            } else {
                // second write only sets low byte
                v = (temp | data);
                t = v;
            }
            w = !w;
            break;
        case 0x2007:    // PPUDATA
            if (v < 0x2000) {
                // Write to pattern table (if the cartdrige supports it)
                if (nes->cart.chrRom_Size == 0) {
                    ppuWrite(v, data);
                }
            } else if (v < 0x3F00) {
                // write to nametables
                if (nes->cart.nametableArr == 0) {
                    // vertical arrangement or horizontal mirroring, we do an and with $800 because if not then all addresses would mirror to the first $400 bytes
                    VRAM[(v & 0x3FF) | ((v & 0x800) >> 1)] = data;

                } else {
                    // horizontal arrangement or vertical mirroring
                    VRAM[v & 0x7FF] = data;
                }
            } else {
                // write to palette ram
                // if index 0 of the palette then use mirror
                if ((v & 0x03) == 0x00) {
                    // Universal background color: all $3F00, $3F04, $3F08, $3F0C, $3F10, $3F14, $3F18, $3F1C map to paletteRAM[0]
                    paletteRAM[0] = data;
                } else {
                    // All other palette entries use direct indexing
                    paletteRAM[v & 0x1F] = data;
                }
                std::ofstream palLog("palette_writes.log", std::ios::app);
                if (palLog.is_open()) {
                    palLog << "PPU write to palette v=0x" << std::hex << v
                           << " idx=" << std::dec << (int)(v & ((v & 3) == 0 ? 0x0F : 0x1F))
                           << " data=0x" << std::hex << (int)data
                           << " ctrl.vramInc=" << std::dec << (int)ctrl.vramInc
                           << "\n";
                }
            }
            v += uint16_t(ctrl.vramInc ? 32 : 1);
            v &= 0x3FFF;
            break;
        default:
            break;
    }
}

uint8_t bugPpu::readPPU(uint16_t address) {
    if (address < 0x2000) {
        return ppuRead(address);
    }
    else if (address < 0x3F00) {
        // Read from the Nametables and we once again check for horizontal or vertical mirroring
        if (nes->cart.nametableArr == 0) {
            // Horizontal
            return VRAM[(address & 0x3FF) | ((address & 0x800) >> 1)];
        } else {
            // Vertical
            return VRAM[address & 0x7FF];
        }
    }
    else {
        // Read from Palette RAM
        if ((address & 3) == 0) {
            return paletteRAM[address & 0x0F];
        }
        else {
            return paletteRAM[address & 0x1F];
        }
    }
}

// This function is for when the CPU reads FROM the PPU
uint8_t bugPpu::cpuRead(uint16_t address) {
    switch (address) {
        case 0x2002:    //PPUSTATUS
            temp8 = status.vBlank == 1 ? 0x80 : 0; // lower 5 bits are open bus
            temp8 |= 0x40;
            status.vBlank = 0;
            w = false;
            return temp8;
        case 0x2007: {
            uint8_t ret = ppuReadBuffer;
            if (v < 0x2000) {
                ppuReadBuffer = ppuRead(v);
            }
            else if (v < 0x3F00) {
                // Read from the Nametables and we once again check for horizontal or vertical mirroring
                if (nes->cart.nametableArr == 0) {
                    // Horizontal
                    ppuReadBuffer = VRAM[(v & 0x3FF) | ((v & 0x800) >> 1)];
                } else {
                    // Vertical
                    ppuReadBuffer = VRAM[v & 0x7FF];
                }
            }
            else {
                // Read from Palette RAM
                if ((v & 3) == 0) {
                    ret = paletteRAM[0];
                }
                else {
                    ret = paletteRAM[v & 0x1F];
                }
            }

            v += uint16_t(ctrl.vramInc ? 32 : 1);
            v &= 0x3FFF;
            return ret;
        }
        default:
            return 0;
    }
}


uint8_t bugPpu::getIndex(uint8_t x, uint8_t y) {
    lowByte = ppuRead(y);
    return lowByte;
}

uint32_t bugPpu::getColor(uint8_t idx) {
    idx &= 0x3F;
    return nesPalette[paletteRAM[idx]];
}

void bugPpu::setPixel(uint8_t x, uint8_t y, uint32_t pixelColor) {
    if (x >= 0 && x < 256 && y >= 0 && y < 240) {
        nes->screenBuffer[(y << 8) | x] = pixelColor;
    }
}

void bugPpu::drawPatternTable() {
    for (int table = 0; table < 2; table++) {
        for (int row = 0; row < 16; row++) {
            for (int col = 0; col < 16; col++) {
                for (int y = 0; y < 8; y++) {
                    lowByte = ppuRead(y + col*16 + row*256 + table*4096);
                    highByte = ppuRead(8 + y + col*16 + row*256 + table*4096);
                    for (int x = 0; x < 8; x++) {
                        twoBit = ((lowByte >> (7-x)) & 1) == 1 ? 1 : 0;
                        twoBit += ((highByte>>(7-x)) & 1) == 1 ? 2 : 0;
                        color = getColor(twoBit);
                        setPixel(x + col*8 + table*128, y + row*8, color);
                    }
                }
            }
        }
    }
    drawNewFrame = true;
}

void bugPpu::drawNametable() {
    for (int row = 0; row < 30; row++) {
        for (int col = 0; col < 32; col++) {
            auto attributeOffset = uint8_t((col >> 2) + (row >> 2) * 8);
            uint8_t attribute = VRAM[0x3C0 + attributeOffset];
            auto quadrant = uint8_t(((col >> 1) & 1) + ((row >> 1) & 1) *2 );
            auto pair = uint8_t((attribute >> (quadrant * 2)) & 0x03);

            uint8_t tileIndex = VRAM[row*32 + col];
            for (int y = 0; y < 8; y++) {
                int useSecondTable = ctrl.backgroundPatternTable ? 4096 : 0;
                lowByte = ppuRead(y + tileIndex*16 + useSecondTable);
                highByte = ppuRead(8 + y + tileIndex*16 + useSecondTable);
                for (int x = 0; x < 8; x++) {
                    twoBit = ((lowByte >> (7-x)) & 1) == 1 ? 1 : 0;
                    twoBit += ((highByte>>(7-x)) & 1) == 1 ? 2 : 0;
                    if (twoBit == 0) {
                        color = getColor(0);
                    }
                    else {
                        color = getColor(twoBit + pair * 4);
                    }
                    setPixel(x + col*8, y + row*8, color);
                }
            }
        }
    }
    drawNewFrame = true;
}

void bugPpu::loadShiftRegisters() {
    shiftRegisterPatternLo  = (shiftRegisterPatternLo  & 0xFF00) | patternLo;
    shiftRegisterPatternHi  = (shiftRegisterPatternHi  & 0xFF00) | patternHi;
    shiftRegisterAttributeLo = (shiftRegisterAttributeLo & 0xFF00) | ((attributeByte & 0x01) == 1 ? 0xFF : 0x00);
    shiftRegisterAttributeHi = (shiftRegisterAttributeHi & 0xFF00) | ((attributeByte & 0x02) == 2 ? 0xFF : 0x00);
}

void bugPpu::ppuClock() {
    if (cycle == 1 && scanline == 241) {
        status.vBlank = 1;
        drawNewFrame = true;
    }
    else if (cycle == 1 && scanline == 261) {
        status.vBlank = 0;
    }

    if ((scanline < 240 || scanline == 261)) {
        // We checked if this is a visible scanline or the pre-render line (scanline 261)
        if ((cycle > 0 && cycle < 256) || (cycle > 320 && cycle <= 336)) {
            // We checked in this case if it's a visible pixel or the start of the next scnaline
            if (mask.showBackground || mask.showSprites) {  // Rendering is enabled
                if (mask.showBackground) {
                    shiftRegisterPatternLo <<= 1;
                    shiftRegisterPatternHi <<= 1;
                    shiftRegisterAttributeLo <<= 1;
                    shiftRegisterAttributeHi <<= 1;
                }
                uint8_t cycleTick = (cycle - 1) & 7;
                switch (cycleTick) {
                    case 0:
                        loadShiftRegisters();
                        ppuAddressBus = uint16_t(0x2000 + (v & 0x0FFF));
                        step8_temp = readPPU(ppuAddressBus);
                        break;
                    case 1:
                        ppuNextCharacter = step8_temp;
                        break;
                    case 2:
                        ppuAddressBus = uint16_t(0x23C0 | (v & 0x0C00) | ((v >> 4 ) & 0x38) | ((v >> 2 ) & 0x07));
                        step8_temp = readPPU(ppuAddressBus);
                        break;
                    case 3:
                        attributeByte = step8_temp;
                        if ((v & 3) >= 2) {
                            attributeByte = attributeByte >> 2;
                        }
                        if ((((v & 0x3E0) >> 5) & 3) >= 2) {
                            attributeByte = attributeByte >> 4;
                        }
                        attributeByte = attributeByte & 0x03;
                    // {
                    //     uint8_t shift = (uint8_t)((((v >> 4) & 4) | ((v >> 2) & 2)));
                    //     attributeByte = (attributeByte >> shift) & 0x03;
                    // }
                        break;
                    case 4:
                        ppuAddressBus = (((v & 0x7000)>> 12) | ppuNextCharacter * 16 | (ctrl.backgroundPatternTable ? 0x1000 : 0));
                        step8_temp = readPPU(ppuAddressBus);
                        break;
                    case 5:
                        patternLo = step8_temp;
                        ppuAddressBus += 8;
                        break;
                    case 6:
                        step8_temp = readPPU(ppuAddressBus);
                        break;
                    case 7:
                        patternHi = step8_temp;
                        if ((v & 0x001F) == 31) {
                            v &= 0xFFE0;
                            v ^= 0x0400;
                        }
                        else {
                            v++;
                        }
                        break;
                }


            }
        }
        if (mask.showBackground || mask.showSprites) {
            // Increment Y at the end of a visible scanline
            if (cycle == 256) {
                incrementScrollY();
            }

            // Reset horizontal position at start of next scanline
            if (cycle == 257) {
                resetScrollX();
            }

            // On pre-render line, restore vertical position (cycles 280–304)
            if (scanline == 261 && cycle >= 280 && cycle <= 304) {
                resetScrollY();
            }
            if (scanline < 241 && cycle <= 256) {
                uint8_t palHi = 0;
                uint8_t palLo = 0;
                if (mask.showBackground && (cycle > 8 || mask.showBackgroundLeft)) {
                    uint8_t col0 =  (shiftRegisterPatternLo >> (15 - scroll.x)) & 1;
                    uint8_t col1 =  (shiftRegisterPatternHi >> (15 - scroll.x)) & 1;
                    palLo = (col1 << 1 ) | col0;

                    uint8_t pal0 =  ((shiftRegisterAttributeLo) >> (15 - scroll.x)) & 1;
                    uint8_t pal1 =  ((shiftRegisterAttributeHi) >> (15 - scroll.x)) & 1;
                    palHi = uint8_t((pal1 << 1) | pal0);

                    if (palLo == 0 && palHi != 0) {
                        palHi = 0;
                    }
                    uint8_t colorByte = (palHi << 2) | palLo;
                    color = getColor(colorByte);
                    setPixel(cycle - 1, scanline, color);
                }
            }
        }
    }

    cycle++;
    if (cycle > 341) {
        cycle = 0;
        scanline++;
        if (scanline > 261) {
            scanline = 0;
        }
    }
}

void bugPpu::incrementScrollY() {
    if ((v & 0x7000) != 0x7000) {
        v += 0x1000;
    }
    else {
        v &= 0x0FFF;
        int y = (v & 0x03E0) >> 5;
        if (y == 29) {
            y = 0;
            v ^= 0x0800;    // Reset the y value and flip the 11th bit in the v register
        }
        else {
            y++;
            y &= 0x1F;  // we make sure that its only the 6 bits we care about
        }
        v = (uint16_t)((v & 0xFC1F) | (y << 5));
    }
}

void bugPpu::resetScrollX() {
    v = (uint16_t)((v & 0xFBE0) | (t & 0x041F));
}

void bugPpu::resetScrollY() {
    v = (uint16_t)((v & 0x041F) | (t & 0x7EB0));
}

void bugPpu::dumpVRAMToFile(const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }

    // Dump nametables
    file << "=== NAMETABLES ===\n";
    for (int nt = 0; nt < 2; nt++) {
        file << "\nNametable " << nt << ":\n";
        uint16_t offset = nt * 0x400;
        for (int row = 0; row < 30; row++) {
            for (int col = 0; col < 32; col++) {
                file << std::hexfloat << std::setw(2) << std::setfill('0')
                     << (int)VRAM[offset + row * 32 + col] << " ";
            }
            file << "\n";
        }
    }

    // Dump attribute tables
    file << "\n=== ATTRIBUTE TABLES ===\n";
    for (int at = 0; at < 2; at++) {
        file << "\nAttribute Table " << at << ":\n";
        uint16_t offset = at * 0x400 + 0x3C0;
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                file << std::hexfloat << std::setw(2) << std::setfill('0')
                     << (int)VRAM[offset + row * 8 + col] << " ";
            }
            file << "\n";
        }
    }

    // Dump palette RAM
    file << "\n=== PALETTE RAM ===\n";
    for (int i = 0; i < 32; i++) {
        if (i % 16 == 0) file << "\n";
        file << std::hexfloat << std::setw(2) << std::setfill('0')
             << (int)paletteRAM[i] << " ";
    }
    file << "\n";

    file.close();
}
