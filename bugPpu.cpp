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

uint8_t bugPpu::ppuRead(uint16_t address) {
    return nes->ppuRead(address);
}

void bugPpu::ppuWrite(uint16_t address, uint8_t data) {
    nes->ppuWrite(address, data);
}

void bugPpu::cpuWrite(uint16_t address, uint8_t data) {
    switch (address) {
        case 0x2000:    // PPUCTRL
            break;
        case 0x2001:    // PPUMASK
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
            v += uint16_t(vramInc32Mode ? 32 : 1);
            v &= 0x3FFF;
            break;
        default:
            break;
    }
}

uint8_t bugPpu::getIndex(uint8_t x, uint8_t y) {
    lowByte = ppuRead(y);
    return lowByte;
}

uint32_t bugPpu::getColor(uint8_t idx) {
    idx &= 0x3F;
    return nesPalette[idx];
}

void bugPpu::setPixel(uint8_t x, uint8_t y, uint32_t color) {
    if (x >= 0 && x < 256 && y >= 0 && y < 240) {
        nes->screenBuffer[(y << 8) | x] = color;
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

void bugPpu::ppuClock() {
    index = getIndex(cycle, scanline);
    color = getColor(index);
    setPixel(cycle, scanline, color);
    if (cycle == 255) {
        cycle = 0;
        scanline++;
        return;
    }
    cycle++;
}

