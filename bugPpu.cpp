//
// Created by felipe on 22/03/26.
//

#include "bugPpu.h"

bugPpu::bugPpu() {

}

bugPpu::~bugPpu() {

}

uint8_t bugPpu::getIndex(uint8_t x, uint8_t y) {

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

