#include "ppu/fetcher.h"
#include "ppu/ppu.h"

#include <iostream>

void PixelFetcher::tick() {
    ticks++;
    if (ticks < 2) {
        return;
    }

    ticks = 0;

    switch (state) {
        case FetcherState::READ_TILE_ID:
            read_tile_id();
            break;
        case FetcherState::READ_FIRST_BYTE:
            read_first_byte();
            break;
        case FetcherState::READ_SECOND_BYTE:
            read_second_byte();
            break;
        case FetcherState::PUSH_TO_FIFO:
            push_to_fifo();
            break;
    }
}

void PixelFetcher::read_tile_id() {

    uint16_t map_addr = ppu->bg_tile_map_area().first;
    uint16_t offset = (ppu->registers[PPU::LY]/8 * 32);

    //std::cout << std::format("LY: {:02X}, X: {:02X}, Dots: {:03}, Tile: {:02X}", ppu->registers[PPU::LY], ppu->x_coord, ppu->dots-80, tile_index) << std::endl;

    tile_id = ppu->read_vram(map_addr + offset + 2);
    state = FetcherState::READ_FIRST_BYTE;
}

void PixelFetcher::read_first_byte() {
    uint16_t data_addr = ppu->tile_data_area().first;
    uint16_t tile_line = ppu->registers[PPU::LY] % 8;
    uint16_t addr = data_addr + (tile_line * 2) + (tile_id * 16);

    byte1 = ppu->read_vram(addr);
    state = FetcherState::READ_SECOND_BYTE;
}

void PixelFetcher::read_second_byte() {
    uint16_t data_addr = ppu->tile_data_area().first;
    uint16_t tile_line = ppu->registers[PPU::LY] % 8;
    uint16_t addr = data_addr + (tile_line * 2) + (tile_id * 16);

    byte2 = ppu->read_vram(addr+1);

    if (delay) {
        tile_index++;
        delay = false;
        state = FetcherState::READ_TILE_ID;
    } else {
        state = FetcherState::PUSH_TO_FIFO;
    }
}

void PixelFetcher::push_to_fifo() {

    if (FIFO.size() > 8) {
        return;
    }

    uint8_t low, high;
    uint8_t pixel;

    for (int j = 0; j < 8; j++) {
        low = (byte1 >> (7 - j)) & 0x01;
        high = (byte2 >> (7 - j)) & 0x01;

        pixel = (high << 1) | low; 
        FIFO.push(pixel);
    }

    tile_index = (tile_index + 1) % 20;
    state = FetcherState::READ_TILE_ID;
}

void PixelFetcher::reset() {
    state = FetcherState::READ_TILE_ID;
    tile_index = 0;
    ticks = 0;
    delay = true;
}

uint8_t PixelFetcher::fetch() {
        return FIFO.pop();
}

bool PixelFetcher::has_pixels() {
    return FIFO.size() > 0;
}
