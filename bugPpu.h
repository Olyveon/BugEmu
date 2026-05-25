//
// Created by felipe on 22/03/26.
//

#ifndef BUGEMU_BUGPPU_H
#define BUGEMU_BUGPPU_H

#pragma once
#include <array>
#include <cstdint>

class bugNES;

class bugPpu {
    public:
    bugPpu();
    ~bugPpu();

    std::array<uint8_t, 0x800> VRAM {};
    std::array<uint8_t, 0x20> paletteRAM {};

    void ConnectSystem(bugNES *n) { nes = n; }
    void reload();

    int scanline {};    // Rows (y coordinate), goes from 0 to 239 (that are visible)
    int cycle {};       // Columns (x coordinate), goes from 0 to 255 (that are visible), one pixel is updated every clock cycle
    uint8_t cycleTick {}; // Determines in which step of the 8 step process is the ppu cycle
    uint32_t color {};
    uint8_t index {};
    uint16_t shiftRegisterPatternLo {};
    uint16_t shiftRegisterPatternHi {};
    uint16_t shiftRegisterAttributeLo {};
    uint16_t shiftRegisterAttributeHi {};

    union ppuStatus {
        struct {
            uint8_t unused : 5;
            uint8_t spriteOverflow : 1;
            uint8_t sprite0Hit : 1;
            uint8_t vBlank : 1;
        };
        uint8_t value;
    } status;

    union ppuCtrl {
        struct {
            uint8_t nametable : 2 ;
            uint8_t vramInc : 1;
            uint8_t spritePatternTable : 1;
            uint8_t backgroundPatternTable : 1;
            uint8_t spriteSize : 1;
            uint8_t slaveMode : 1;
            uint8_t enableNMI : 1;
        };
        uint8_t value;
    } ctrl;

    union ppuMask {
        struct {
            uint8_t grayscale : 1;
            uint8_t showBackgroundLeft : 1;
            uint8_t showSpritesLeft : 1;
            uint8_t showBackground : 1;
            uint8_t showSprites : 1;
            uint8_t enhanceRed : 1;
            uint8_t enhanceGreen : 1;
            uint8_t enhanceBlue : 1;
        };
        uint8_t value;
    } mask;



    // To encounter index
    uint8_t lowByte {};
    uint8_t highByte {};
    uint8_t twoBit {};
    uint16_t temp {};       // used for multiple operations as a temporal value
    uint8_t temp8 {};
    uint8_t tempByte {};

    // for the rendering process
    uint8_t ppuReadBuffer {};
    uint16_t ppuAddressBus {};
    uint8_t ppuNextCharacter {};


    // Ppu internal registers
    bool w {};          // Write Latch
    uint16_t t {};      // Transfer address
    uint16_t v {};      // VRAM address

    uint8_t ppuRead(uint16_t address);
    uint8_t readPPU(uint16_t address);
    void ppuWrite(uint16_t address, uint8_t data);

    void cpuWrite(uint16_t address, uint8_t data);
    uint8_t cpuRead(uint16_t address);

    uint8_t getIndex(uint8_t x, uint8_t y);
    uint32_t getColor(uint8_t idx); // from an x and y index it gets the color that it should be drawn as in RGBA8888 format for SDL3
    void setPixel(uint8_t x, uint8_t y, uint32_t pixelColor);
    void drawPatternTable();
    void drawNametable();

    void ppuClock();

    private:

    bugNES *nes = nullptr;
    const uint32_t nesPalette[64];

    void loadShiftRegisters();
    uint16_t patternLo;
    uint16_t patternHi;
    uint8_t attributeByte;

};


#endif //BUGEMU_BUGPPU_H