#include "ppu/ppu.h"

bool PPU:: lcd_ppu_enabled() {
    return (registers[LCDC] & 128) == 128;
}

MemRange PPU::window_tile_map_area() {
    if ((registers[LCDC] & 64) == 0) {
        return {MAP_AREA_0_START, MAP_AREA_0_END};
    }

    return {MAP_AREA_1_START, MAP_AREA_1_END};
}

bool PPU::window_enabled() {
    return (registers[LCDC] & 32) == 32;
}

MemRange PPU::tile_data_area() {
    if ((registers[LCDC] & 16) == 0) {
        return {DATA_AREA_0_START, DATA_AREA_0_END};
    }

    return {DATA_AREA_1_START, DATA_AREA_1_END};
}

bool PPU::signed_mode() {
    return (registers[LCDC] & 16) == 0;
}

MemRange PPU::bg_tile_map_area() {
    if ((registers[LCDC] & 8) == 0) {
        return {MAP_AREA_0_START, MAP_AREA_0_END};
    }

    return {MAP_AREA_1_START, MAP_AREA_1_END};
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
