#include <ppu/ppu.h>
#include <iostream>

uint8_t PPU::read_reg_default(uint16_t loc) {
    return registers[loc];
}

void PPU::write_reg_default(uint16_t loc, uint8_t data) {
    registers[loc] = data;
}

uint8_t PPU::read_vram(uint16_t loc) {

    if (loc < VRAM_START || loc > VRAM_END) {
        throw std::runtime_error(std::format("VRAM read out of bounds: {:04X}", loc));
    }

    return vram[loc - VRAM_START];
}

uint8_t PPU::read_oam(uint16_t loc) {

    if (loc < OAM_START || loc > OAM_END) {
        throw std::runtime_error(std::format("OAM read out of bounds: {:04X}", loc));
    }

    return oam[loc - OAM_START];
}

uint8_t PPU::read_register(uint16_t loc) {

    if (loc < REG_START || loc > REG_END) {
        throw std::runtime_error(std::format("PPU register read out of bounds: {:04X}", loc));
    }

    uint16_t shifted_loc = loc - REG_START;
    ReadHandler handler = read_reg_handlers[shifted_loc];
    return (this->*handler)(shifted_loc);
}

uint8_t PPU::read_register(PPU_REG reg) {
    uint16_t loc = static_cast<uint16_t>(reg) - REG_START;
    ReadHandler handler = read_reg_handlers[loc];
    return (this->*handler)(loc);
}

void PPU::write_vram(uint16_t loc, uint8_t data) {

    if (loc < VRAM_START || loc > VRAM_END) {
        throw std::runtime_error(std::format("VRAM write out of bounds: {:04X}", loc));
    }

    vram[loc - VRAM_START] = data;
}

void PPU::write_oam(uint16_t loc, uint8_t data) {

    if (loc < OAM_START || loc > OAM_END) {
        throw std::runtime_error(std::format("OAM write out of bounds: {:04X}", loc));
    }

    oam[loc - OAM_START] = data;
}

void PPU::write_register(uint16_t loc, uint8_t data) {

    if (loc < REG_START || loc > REG_END) {
        throw std::runtime_error(std::format("PPU register write out of bounds: {:04X}", loc));
    }

    uint16_t shifted_loc = loc - REG_START;
    WriteHandler handler = write_reg_handlers[shifted_loc];
    (this->*handler)(shifted_loc, data);
}

void PPU::write_register(PPU_REG reg, uint8_t data) {
    uint16_t loc = static_cast<uint16_t>(reg) - REG_START;
    WriteHandler handler = write_reg_handlers[loc];
    (this->*handler)(loc, data);
}

void PPU::set_write_handlers() {
    uint16_t ly   = static_cast<uint16_t>(PPU_REG::LY) - REG_START;
    uint16_t stat = static_cast<uint16_t>(PPU_REG::STAT) - REG_START;

    write_reg_handlers[ly] = &PPU::write_LY;
    write_reg_handlers[stat] = &PPU::write_stat;
}

void PPU::set_read_handlers() {
    uint16_t stat = static_cast<uint16_t>(PPU_REG::STAT) - REG_START;

    read_reg_handlers[stat] = &PPU::read_stat;
}

void PPU::initialize_registers() {
    write_register(PPU_REG::LCDC, 0x91);
    write_register(PPU_REG::STAT, 0x85);
    write_register(PPU_REG::BGP, 0xFC);
}

void PPU::write_LY(uint16_t loc, uint8_t data) {
    return;
}

void PPU::inc_LY() {
    // bypass write_register since LY is unwritable
    uint8_t addr = static_cast<uint16_t>(PPU_REG::LY) - REG_START;
    registers[addr]++;
    compare();
}

void PPU::reset_LY() {
    // bypass write_register since LY is unwritable
    uint8_t addr = static_cast<uint16_t>(PPU_REG::LY) - REG_START;
    registers[addr] = 0;
    fetcher.reset_window();
    compare();
}

void PPU::write_stat(uint16_t loc, uint8_t data) {
    uint8_t old = registers[loc];
    registers[loc] = (data & 0xF8) | (old & 0x07);
}

uint8_t PPU::read_stat(uint16_t loc) {
    uint8_t stat = registers[loc];

    uint8_t ly = read_register(PPU_REG::LY);
    uint8_t lyc = read_register(PPU_REG::LYC);

    uint8_t compare = ly == lyc;

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
