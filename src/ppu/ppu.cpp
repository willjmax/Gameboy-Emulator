#include "ppu/ppu.h"
#include <iostream>

PPU::PPU(Interrupt& i) : 
    interrupt(i), fetcher(this) {
    for (int i = 0; i < REG_SIZE; ++i) {
        registers[i] = 0;
        write_reg_handlers[i] = &PPU::write_reg_default;
        read_reg_handlers[i] = &PPU::read_reg_default;
    }

    for (int i = 0; i < VRAM_SIZE; ++i) {
        vram[i] = 0;
    }

    for (int i = 0; i < OAM_SIZE; ++i) {
        oam[i] = 0;
    }

    set_read_handlers();
    set_write_handlers();
    initialize_registers();
}

void PPU::tick(uint8_t cycles) {

    if (!lcd_ppu_enabled()) {
        return;
    }

    for (int i = 0; i < cycles; i++) {
        dots++;

        switch (mode) {
            case PPU_Mode::HBLANK:
                mode_0_hblank();
                break;
            case PPU_Mode::VBLANK:
                mode_1_vblank();
                break;
            case PPU_Mode::OAM_SCAN:
                mode_2_oam_scan();
                break;
            case PPU_Mode::DRAWING:
                mode_3_drawing();
                break;
        }
    }
}

void PPU::mode_0_hblank() {
    if (dots < 456) {
        return;
    }

    dots = 0;
    inc_LY();
    if (read_register(PPU_REG::LY) == 144) {
        interrupt.request_vblank_interrupt();
        frame_ready = true;
        mode = PPU_Mode::VBLANK;
    } else {
        mode = PPU_Mode::OAM_SCAN;
    }

}

void PPU::mode_1_vblank() {
    if (dots < 456) {
        return;
    }

    dots = 0;
    if (read_register(PPU_REG::LY) == 153) {
        reset_LY();
        mode = PPU_Mode::OAM_SCAN;
    } else {
        inc_LY();
    }
}

void PPU::mode_2_oam_scan() {
    if (dots < 80) {
        return;
    }

    sprite_buffer.clear();

    for (uint16_t offset = 0; offset < 160; offset += 4) {
        Sprite sprite = fetch_sprite(offset);
        int size = obj_size();

        if (sprite.on_scanline(read_register(PPU_REG::LY), size)){
            sprite_buffer.push_back(sprite);
        }

        if (sprite_buffer.size() == 10) {
            break;
        }
    }

    x_coord = 0;
    scx_cnt = 0;
    mode = PPU_Mode::DRAWING;
}

void PPU::mode_3_drawing() {
    auto sprite = sprite_on_column();

    if (sprite.has_value()) {
        fetcher.request_obj_mode(sprite.value());
    }

    fetcher.tick();

    auto pixel = fetcher.select();
    if (pixel.has_value()) {
        uint8_t color = color_from_pixel(pixel.value());
        write_to_framebuffer(x_coord, read_register(PPU_REG::LY), color);
        x_coord++;
    }

    if (window_enabled() &&
        fetcher.fetcher_mode() == FetcherMode::BACKGROUND &&
        read_register(PPU_REG::LY) >= read_register(PPU_REG::WY) &&
        x_coord >= read_register(PPU_REG::WX) - 7) 
    {
        fetcher.reset(FetcherMode::WINDOW);
        fetcher.inc_window();
    }

    if (x_coord == 160) {
        fetcher.reset(FetcherMode::BACKGROUND);
        mode = PPU_Mode::HBLANK;
    }
}

void PPU::write_to_framebuffer(int x, int y, uint8_t pixel) {
    int loc = (y * WIDTH) + x;
    framebuffer[loc] = pixel;
}

void PPU::compare() {

    if ((read_register(PPU_REG::STAT) & 6) == 0) {
        return;
    }

    if (read_register(PPU_REG::LY) == read_register(PPU_REG::LYC)) {
        interrupt.request_stat_interrupt();
    }
}

Sprite PPU::fetch_sprite(uint16_t offset) {
    uint8_t y_pos = oam[offset+0];
    uint8_t x_pos = oam[offset+1];
    uint8_t index = oam[offset+2];
    uint8_t attrs = oam[offset+3];

    return Sprite(y_pos, x_pos, index, attrs);
}

std::optional<Sprite> PPU::sprite_on_column() {
    for (int i = 0; i < (int)sprite_buffer.size(); i++) {
        Sprite sprite = sprite_buffer[i];
        if (sprite.x_pos() == x_coord + 8) {
            return sprite; 
        }
    }

    return std::nullopt;
}

uint8_t PPU::color_from_pixel(Pixel pixel) {
    uint8_t palette = read_register(pixel.palette);
    uint8_t offset = pixel.color_id * 2;

    return (palette >> offset) & 0x03;
}
