#include "ppu/ppu.h"

void PPU::write_stat(uint16_t loc, uint8_t data) {
    uint8_t old = registers[loc];
    registers[loc] = (data & 0xF8) | (old & 0x07);
}

uint8_t PPU::read_stat(uint16_t loc) {
    uint8_t stat = registers[loc];

    uint8_t compare = registers[LY] == registers[LYC];

    stat |= 128;
    stat |= (compare << 2);

    if (!lcd_ppu_enabled()) {
        stat &= 0xFE;
    }

    stat &= ~0x03;

    switch (mode) {
        case PPU_Mode::HBLANK:
            stat |= 0x00;
            break;
        case PPU_Mode::VBLANK:
            stat |= 0x01;
            break;
        case PPU_Mode::OAM_SCAN:
            stat |= 0x02;
            break;
        case PPU_Mode::DRAWING:
            stat |= 0x03;
            break;
    }

    return stat;
}
