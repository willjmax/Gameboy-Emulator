#include <optional>
#include <stdexcept>
#include "ppu/fetcher.h"
#include "ppu/ppu.h"
#include "ppu/registers.h"

void PixelFetcher::tick() {

    ticks++;
    if (ticks < 2) {
        return;
    }

    ticks = 0;

    if (mode == FetcherMode::OBJECT) {
        switch (obj_state) {
            case OBJ_State::GET_SPRITE_TILE:
                get_sprite_tile();
                break;
            case OBJ_State::GET_SPRITE_HIGH:
                get_sprite_high();
                break;
            case OBJ_State::GET_SPRITE_LOW:
                get_sprite_low();
                break;
            case OBJ_State::MERGE_FIFO:
                merge_fifo();
                break;
        }
    } else {
        switch (bg_state) {
            case BG_State::READ_TILE_ID:
                read_tile_id();
                break;
            case BG_State::READ_FIRST_BYTE:
                read_first_byte();
                break;
            case BG_State::READ_SECOND_BYTE:
                read_second_byte();
                break;
            case BG_State::PUSH_TO_FIFO:
                push_to_fifo();
                break;
        }
    }
}

// bg/window mode

void PixelFetcher::read_tile_id() {
    uint16_t map_addr;
    uint16_t tile_x;
    uint16_t tile_y;
    uint16_t offset;

    uint8_t ly = ppu->read_register(PPU_REG::LY);
    uint8_t scx = ppu->read_register(PPU_REG::SCX);
    uint8_t scy = ppu->read_register(PPU_REG::SCY);

    switch (mode) {
        case FetcherMode::BACKGROUND:
            map_addr = ppu->bg_tile_map_area();
            tile_x = (tile_index + (scx/8)) & 0x1F;
            tile_y = (ly + scy)/8 & 0x1F;
            break;
        case FetcherMode::WINDOW:
            map_addr = ppu->window_tile_map_area();
            tile_x = tile_index;
            tile_y = window_count/8;
            break;
        case FetcherMode::OBJECT:
            throw std::runtime_error("Bad state: FetcherMode::OBJECT");
    }

    offset = tile_x + tile_y*32;
    tile_id = ppu->read_vram(map_addr + offset);
    bg_state = BG_State::READ_FIRST_BYTE;
}

void PixelFetcher::read_first_byte() {
    uint8_t ly = ppu->read_register(PPU_REG::LY);
    uint8_t scy = ppu->read_register(PPU_REG::SCY);

    uint16_t tile_line = (ly + scy) % 8;
    uint16_t addr;

    if (ppu->signed_mode()) {
        int8_t signed_id = static_cast<int8_t>(tile_id);
        addr = PPU::DATA_AREA_0_START + tile_line*2 + signed_id*16;
    } else {
        addr = PPU::DATA_AREA_1_START + tile_line*2 + tile_id*16;
    }

    byte1 = ppu->read_vram(addr);
    bg_state = BG_State::READ_SECOND_BYTE;
}

void PixelFetcher::read_second_byte() {
    uint8_t ly = ppu->read_register(PPU_REG::LY);
    uint8_t scy = ppu->read_register(PPU_REG::SCY);

    uint16_t tile_line = (ly + scy) % 8;
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
        bg_state = BG_State::READ_TILE_ID;
    } else {
        bg_state = BG_State::PUSH_TO_FIFO;
    }
}

void PixelFetcher::push_to_fifo() {

    if (obj_requested) {
        prev_mode = mode;
        mode = FetcherMode::OBJECT;
        obj_state = OBJ_State::GET_SPRITE_TILE;
        obj_requested = false;
    }

    if (BG_FIFO.size() > 8) {
        return;
    }

    uint8_t low, high;
    uint8_t color_id;
    Pixel pixel;

    for (int j = 0; j < 8; j++) {
        low = (byte1 >> (7 - j)) & 0x01;
        high = (byte2 >> (7 - j)) & 0x01;

        color_id = (high << 1) | low; 
        color_id = color_id_lookup(PPU_REG::BGP, color_id);

        pixel = Pixel(color_id, false);
        BG_FIFO.push(pixel);
    }

    tile_index++;
    bg_state = BG_State::READ_TILE_ID;
}

void PixelFetcher::reset(FetcherMode f_mode) {
    bg_state = BG_State::READ_TILE_ID;
    tile_index = 0;
    mode = f_mode;
    ticks = 0;
    delay = true;
    BG_FIFO.clear();
}

// object mode

void PixelFetcher::request_obj_mode(Sprite sprite) {
    obj_requested = true;
    oam_sprite = sprite;
}

void PixelFetcher::get_sprite_tile() {
    uint8_t tile_index = oam_sprite.tile_index();
    uint8_t ly = ppu->read_register(PPU_REG::LY);
    uint8_t sprite_y_offset = ly + 16 - oam_sprite.y_pos();

    tile_id = tile_index*16 + sprite_y_offset*2;
    obj_state = OBJ_State::GET_SPRITE_LOW;
}

void PixelFetcher::get_sprite_low() {
    byte1 = ppu->read_vram(tile_id);
    obj_state = OBJ_State::GET_SPRITE_HIGH;
}

void PixelFetcher::get_sprite_high() {
    byte2 = ppu->read_vram(tile_id+1);
    obj_state = OBJ_State::MERGE_FIFO;
}

void PixelFetcher::merge_fifo() {
    if (OBJ_FIFO.size() > 8) {
        return;
    }

    Pixel pixel;
    uint8_t low;
    uint8_t high;
    uint8_t color_id;

    OBJ_FIFO.clear();
    for (int j = 0; j < 8; j++) {
        low = (byte1 >> (7 - j)) & 0x01;
        high = (byte2 >> (7 - j)) & 0x01;

        color_id = (high << 1) | low; 
        color_id = color_id_lookup(oam_sprite.palette(), color_id);
        pixel = Pixel(color_id, oam_sprite.priority());
        OBJ_FIFO.push(pixel);
    }

    obj_state = OBJ_State::GET_SPRITE_TILE;
    mode = prev_mode;
    bg_state = BG_State::READ_TILE_ID;
}

std::optional<Pixel> PixelFetcher::select() {

    if (mode == FetcherMode::OBJECT) {
        return std::nullopt;
    }

    auto bg_pixel = BG_FIFO.pop();
    auto obj_pixel = OBJ_FIFO.pop();

    if (!ppu->bg_window_enabled() && bg_pixel.has_value()) {
        bg_pixel.value().color_id = 0x00;
    }

    uint8_t scx = ppu->read_register(PPU_REG::SCX);
    if (ppu->scx_cnt < scx % 8) {
        ppu->scx_cnt++;
        return std::nullopt;
    }

    if (!bg_pixel.has_value()) {
        return obj_pixel;
    }

    if (!obj_pixel.has_value()) {
        return bg_pixel;
    }

    if (!ppu->bg_window_enabled()) {
        return obj_pixel;
    }

    if (!ppu->obj_enable()) {
        return bg_pixel;
    }

    bool obj_priority = obj_pixel.value().priority;
    uint8_t bg_color_id = bg_pixel.value().color_id;
    uint8_t obj_color_id = obj_pixel.value().color_id;

    if (obj_priority && bg_color_id != 0x00) {
        return bg_pixel;
    }

    if (obj_color_id == 0x00) {
        return bg_pixel;
    } else {
        return obj_pixel;
    }

    return std::nullopt;
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

uint8_t PixelFetcher::color_id_lookup(PPU_REG palette_reg, uint8_t bits) {
    uint8_t palette = ppu->read_register(palette_reg);
    uint8_t offset = bits * 2;

    return (palette >> offset) & 0x03;
}
