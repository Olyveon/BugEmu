//
// Created by felipe on 22/03/26.
//

#ifndef BUGEMU_BUGPPU_H
#define BUGEMU_BUGPPU_H

#pragma once
#include <cstdint>

class bugPpu {
    public:
    bugPpu();
    ~bugPpu();

    uint8_t scanline {};    // Rows (y coordinate), goes from 0 to 239
    uint8_t cycle {};       // Columns (x coordinate), goes from 0 to 255, one pixel is updated every clock cycle
    uint32_t color;
    uint8_t index;

    uint8_t ppuRead(uint16_t address);

    uint8_t getIndex(uint8_t x, uint8_t y);
    uint32_t getColor(uint8_t index); // from an x and y index it gets the color that it should be drawn as in RGBA8888 format for SDL3
    void setPixel(uint8_t x, uint8_t y, uint32_t color);

    void ppuClock();

};


#endif //BUGEMU_BUGPPU_H