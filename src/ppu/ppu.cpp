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

uint8_t PPU::read_reg_default(uint16_t loc) {
    return registers[loc];
}

void PPU::write_reg_default(uint16_t loc, uint8_t data) {
    registers[loc] = data;
}

uint8_t PPU::read_vram(uint16_t loc) {

    if (vram_blocked) {
        return 0xFF;
    }

    return vram[loc];
}

uint8_t PPU::read_oam(uint16_t loc) {
    return oam[loc];
}

uint8_t PPU::read_register(uint16_t loc) {
    uint16_t shifted_loc = loc - REG_START;
    ReadHandler handler = read_reg_handlers[shifted_loc];
    return (this->*handler)(shifted_loc);
}

void PPU::write_vram(uint16_t loc, uint8_t data) {
    vram[loc - VRAM_START] = data;
}

void PPU::write_oam(uint16_t loc, uint8_t data) {
    oam[loc - OAM_START] = data;
}

void PPU::write_register(uint16_t loc, uint8_t data) {
    uint16_t shifted_loc = loc - REG_START;
    WriteHandler handler = write_reg_handlers[shifted_loc];
    (this->*handler)(shifted_loc, data);
}

void PPU::write_LY(uint16_t loc, uint8_t data) {
    return;
}

void PPU::inc_LY() {
    registers[LY]++;
    compare();
}

void PPU::reset_LY() {
    registers[LY] = 0;
    compare();
}

void PPU::set_write_handlers() {
    write_reg_handlers[LY] = &PPU::write_LY;
    write_reg_handlers[STAT] = &PPU::write_stat;
}

void PPU::set_read_handlers() {
    read_reg_handlers[STAT] = &PPU::read_stat;
}

void PPU::initialize_registers() {
    registers[LCDC] = 0x91;
    registers[STAT] = 0x85;
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
    if (dots == 456) {
        dots = 0;
        inc_LY();
        if (registers[LY] == 144) {
            interrupt.request_vblank_interrupt();
            frame_ready = true;
            mode = PPU_Mode::VBLANK;
        } else {
            mode = PPU_Mode::OAM_SCAN;
        }
    }

}

void PPU::mode_1_vblank() {
    if (dots == 456) {
        dots = 0;
        if (registers[LY] == 153) {
            reset_LY();
            mode = PPU_Mode::OAM_SCAN;
        } else {
            inc_LY();
        }
    }
}

void PPU::mode_2_oam_scan() {
    if (dots == 80) {
        x_coord = 0;
        mode = PPU_Mode::DRAWING;
    }
}

void PPU::mode_3_drawing() {

    fetcher.tick();

    if (fetcher.has_pixels()) {
        uint8_t pixel = fetcher.fetch();
        write_to_framebuffer(x_coord, registers[LY], pixel);
        x_coord++;
    }

    if (x_coord == 160) {
        fetcher.reset();
        mode = PPU_Mode::HBLANK;
    }

}

void PPU::write_to_framebuffer(int x, int y, uint8_t pixel) {
    int loc = (y * WIDTH) + x;
    framebuffer[loc] = pixel;
}

void PPU::compare() {

    if ((registers[STAT] & 6) == 0) {
        return;
    }

    if (registers[LY] == registers[LYC]) {
        interrupt.request_stat_interrupt();
    }
}
