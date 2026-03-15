#include "ppu/fetcher.h"
#include "ppu/ppu.h"

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
    uint16_t map_addr;
    uint16_t tile_x;
    uint16_t tile_y;
    uint16_t offset;

    switch (mode) {
        case FetcherMode::BACKGROUND:
            map_addr = ppu->bg_tile_map_area();
            tile_x = (tile_index + (ppu->registers[PPU::SCX]/8)) & 0x1F;
            tile_y = (ppu->registers[PPU::LY] + ppu->registers[PPU::SCY])/8 & 0x1F;
            break;
        case FetcherMode::WINDOW:
            map_addr = ppu->window_tile_map_area();
            tile_x = tile_index;
            tile_y = window_count/8;
            break;
    }

    offset = tile_x + tile_y*32;
    tile_id = ppu->read_vram(map_addr + offset);
    state = FetcherState::READ_FIRST_BYTE;
}

void PixelFetcher::read_first_byte() {
    uint16_t tile_line = (ppu->registers[PPU::LY] + ppu->registers[PPU::SCY]) % 8;
    uint16_t addr;

    if (ppu->signed_mode()) {
        int8_t signed_id = static_cast<int8_t>(tile_id);
        addr = PPU::DATA_AREA_0_START + tile_line*2 + signed_id*16;
    } else {
        addr = PPU::DATA_AREA_1_START + tile_line*2 + tile_id*16;
    }

    byte1 = ppu->read_vram(addr);
    state = FetcherState::READ_SECOND_BYTE;
}

void PixelFetcher::read_second_byte() {
    uint16_t tile_line = (ppu->registers[PPU::LY] + ppu->registers[PPU::SCY]) % 8;
    uint16_t addr;

    if (ppu->signed_mode()) {
        int8_t signed_id = static_cast<int8_t>(tile_id);
        addr = PPU::DATA_AREA_0_START + tile_line*2 + signed_id*16;
    } else {
        addr = PPU::DATA_AREA_1_START + tile_line*2 + tile_id*16;
    }

    byte2 = ppu->read_vram(addr+1);

    if (delay) {
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

    tile_index++;
    state = FetcherState::READ_TILE_ID;
}

void PixelFetcher::reset(FetcherMode f_mode) {
    state = FetcherState::READ_TILE_ID;
    tile_index = 0;
    mode = f_mode;
    ticks = 0;
    delay = true;
    FIFO.clear();
}

uint8_t PixelFetcher::fetch() {
    return FIFO.pop();
}

bool PixelFetcher::has_pixels() {
    return FIFO.size() > 0;
}

FetcherMode PixelFetcher::fetcher_mode() {
    return mode;
}

void PixelFetcher::inc_window() {
    if (mode == FetcherMode::WINDOW) {
        window_count++;
    }
}

void PixelFetcher::reset_window() {
    window_count = -1;
}
