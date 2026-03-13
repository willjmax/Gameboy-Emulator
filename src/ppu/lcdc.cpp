#include "ppu/ppu.h"

bool PPU:: lcd_ppu_enabled() {
    return (registers[LCDC] & 128) == 128;
}

uint16_t PPU::window_tile_map_area() {
    if ((registers[LCDC] & 64) == 0) {
        return MAP_AREA_0_START;
    }

    return MAP_AREA_1_START;
}

bool PPU::window_enabled() {
    return (registers[LCDC] & 32) == 32;
}

uint16_t PPU::tile_data_area() {
    if ((registers[LCDC] & 16) == 0) {
        return DATA_AREA_0_START;
    }

    return DATA_AREA_1_START;
}

bool PPU::signed_mode() {
    return (registers[LCDC] & 16) == 0;
}

uint16_t PPU::bg_tile_map_area() {
    if ((registers[LCDC] & 8) == 0) {
        return MAP_AREA_0_START;
    }

    return MAP_AREA_1_START;
}

std::pair<int, int> PPU::obj_size() {
    if ((registers[LCDC] & 4) == 0) {
        return {8, 8};
    }

    return {8, 16};
}

bool PPU::obj_enable() {
    return (registers[LCDC] & 2) == 2;
}

bool PPU::bg_window_enabled() {
    return (registers[LCDC] & 1) == 1;
}
