//
// Created by felipe on 22/03/26.
//

#include "bugPpu.h"
#include "bugNES.h"

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
                    VRAM[(v & 0x3FF) | v & 0x800 >> 1] = data;

                } else {
                    // horizontal arrangement or vertical mirroring
                    VRAM[v & 0x7FF] = data;
                }
            } else {
                // write to palette ram
                // if index 0 of the palette then use mirror
                if ((v & 3) == 0) {
                    // background and sprite bytes of index 0 mirrored, e.g. $0 and $10 are mirrors of each other, same for $4 and $14 and $08 and $18, hence why we use mask $0F
                    paletteRAM[v & 0x0F] = data;
                }
                else {
                    paletteRAM[v & 0x1F] = data;
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
    if (v < 0x2000) {
        return ppuRead(v);
    }
    else if (v < 0x3F00) {
        // Read from the Nametables and we once again check for horizontal or vertical mirroring
        if (nes->cart.nametableArr == 0) {
            // Horizontal
            return VRAM[(v & 0x3FF) | v & 0x800 >> 1];
        } else {
            // Vertical
            return VRAM[v & 0x7FF];
        }
    }
    else {
        // Read from Palette RAM
        if ((v & 3) == 0) {
            return paletteRAM[v & 0x0F];
        }
        else {
            return paletteRAM[v & 0x1F];
        }
    }
}

// This function is for when the CPU reads FROM the PPU
uint8_t bugPpu::cpuRead(uint16_t address) {
    switch (address) {
        case 0x2002:    // PPUSTATUS
        {
            temp8 = 0;
            temp8 |= uint8_t(status.value ? 0x80 : 0x00);
            temp8 |= 0x40;
            status.vBlank = 0;
            w = false;
            return temp8;
        }
        case 0x2007:
            temp = ppuReadBuffer;
            if (v < 0x2000) {
                ppuReadBuffer = ppuRead(v);
            }
            else if (v < 0x3F00) {
                // Read from the Nametables and we once again check for horizontal or vertical mirroring
                if (nes->cart.nametableArr == 0) {
                    // Horizontal
                    ppuReadBuffer = VRAM[(v & 0x3FF) | v & 0x800 >> 1];
                } else {
                    // Vertical
                    ppuReadBuffer = VRAM[v & 0x7FF];
                }
            }
            else {
                // Read from Palette RAM
                if ((v & 3) == 0) {
                    temp = paletteRAM[v & 0x0F];
                }
                else {
                    temp = paletteRAM[v & 0x1F];
                }
            }

            v += uint16_t(ctrl.vramInc ? 32 : 1);
            v &= 0x3FFF;
            return temp;
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
}

void bugPpu::loadShiftRegisters() {
    shiftRegisterPatternLo  = (shiftRegisterPatternLo  & 0xFF00) | patternLo;
    shiftRegisterPatternHi  = (shiftRegisterPatternHi  & 0xFF00) | patternHi;
    shiftRegisterAttributeLo = (shiftRegisterAttributeLo & 0xFF00) | ((attributeByte & 0x01) ? 0xFF : 0x00);
    shiftRegisterAttributeHi = (shiftRegisterAttributeHi & 0xFF00) | ((attributeByte & 0x02) ? 0xFF : 0x00);
}

void bugPpu::ppuClock() {
    if (cycle == 1 && scanline == 241) {
        status.vBlank = 1;
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
                switch (cycle % 8) {
                    case 0:
                        patternHi = tempByte;
                        break;
                    case 1:
                        loadShiftRegisters();
                        ppuAddressBus = (0x2000 + (v & 0x0FFF));
                        tempByte = readPPU(ppuAddressBus);
                        break;
                    case 2:
                        ppuNextCharacter = tempByte;
                        break;
                    case 3:
                        ppuAddressBus = (0x23C0 | (v & 0x0C00) | ((v >> 4 ) & 0x38) | ((v >> 2 ) & 0x7));
                        tempByte = readPPU(ppuAddressBus);
                        break;
                    case 4:
                        attributeByte = tempByte;
                        if ((v & 3) >= 2) {
                            attributeByte = attributeByte >> 2;
                        }
                        if ((((v & 0x3E0) >> 5) & 3) >= 2)
                        break;
                    case 5:
                        ppuAddressBus = (((v & 0x7000)>> 12) | ppuNextCharacter * 16 | (ctrl.backgroundPatternTable ? 0x1000 : 0));
                        tempByte = readPPU(ppuAddressBus);
                        break;
                    case 6:
                        patternLo = tempByte;
                        ppuAddressBus += 8;
                        break;
                    case 7:
                        tempByte = readPPU(ppuAddressBus);
                        break;
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

