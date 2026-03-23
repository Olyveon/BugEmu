//
// Created by felipe on 22/03/26.
//

#ifndef BUGEMU_BUGPPU_H
#define BUGEMU_BUGPPU_H

#pragma once
#include <cstdint>

class bugNES;

class bugPpu {
    public:
    bugPpu();
    ~bugPpu();

    void ConnectSystem(bugNES *n) { nes = n; }
    uint8_t scanline {};    // Rows (y coordinate), goes from 0 to 239
    uint8_t cycle {};       // Columns (x coordinate), goes from 0 to 255, one pixel is updated every clock cycle
    uint32_t color {};
    uint8_t index {};
    // To encounter index
    uint8_t lowByte {};
    uint8_t highByte {};
    uint8_t twoBit {};

    uint8_t ppuRead(uint16_t address);

    uint8_t getIndex(uint8_t x, uint8_t y);
    uint32_t getColor(uint8_t idx); // from an x and y index it gets the color that it should be drawn as in RGBA8888 format for SDL3
    void setPixel(uint8_t x, uint8_t y, uint32_t color);
    void drawPatternTable();

    void ppuClock();

    private:

    bugNES *nes = nullptr;
    const uint32_t nesPalette[64];

};


#endif //BUGEMU_BUGPPU_H