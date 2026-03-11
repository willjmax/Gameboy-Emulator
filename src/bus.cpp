#include <cstdint>
#include <iostream>
#include "bus.h"
#include <iostream>

uint8_t Bus::read(uint16_t loc) {

    Bus::tick(4);

    if (loc >= APU_START && loc <= APU_END) {
        return apu.read(loc);
    }

    if (loc >= VRAM_START && loc <= VRAM_END) {
        return ppu.read_vram(loc);
    }

    if (loc >= OAM_START && loc <= OAM_END) {
        return ppu.read_oam(loc);
    }

    if (loc >= PPU_START && loc <= PPU_END) {
        return ppu.read_register(loc);
    }

    switch (loc) {

        // timer
        case DIV:
            return timer.read_div();
        case TIMA:
            return timer.read_tima();
        case TMA:
            return timer.read_tma();
        case TAC:
            return timer.read_tac();

        // interrupts
        case IF_REG:
            return interrupt.read_if_reg();
        case IF_ENABLE:
            return interrupt.read_if_enable();

        default:
            return Bus::memory[loc];
    }
}

void Bus::write(uint16_t loc, uint8_t byte) {

    Bus::tick(4);

    if (loc >= APU_START && loc <= APU_END) {
        apu.write(loc, byte);
        return;
    }

    if (loc >= VRAM_START && loc <= VRAM_END) {
        ppu.write_vram(loc, byte);
        return;
    }

    if (loc >= OAM_START && loc <= OAM_END) {
        ppu.write_oam(loc, byte);
        return;
    }

    if (loc >= PPU_START && loc <= PPU_END) {
        ppu.write_register(loc, byte);
        return;
    }

    switch (loc) {

        // timer
        case DIV:
            timer.reset_div();
            break;
        case TIMA:
            timer.write_tima(byte);
            break;
        case TMA:
            timer.write_tma(byte);
            break;
        case TAC:
            timer.write_tac(byte);
            break;

        // interrupts
        case IF_REG:
            interrupt.write_if_reg(byte);
            break;
        case IF_ENABLE:
            interrupt.write_if_enable(byte);
            break;

        case TERMINAL:
            std::cout << (char)byte << std::flush;
            Bus::memory[loc] = byte;
            break;

        default:
            Bus::memory[loc] = byte;
            break;
    }

}

void Bus::loadROM(std::ifstream& file, uintmax_t size) {
    file.read(reinterpret_cast<char*>(&memory[Bus::ROM_FIXED_START]), size);
}

void Bus::tick(int cycles) {
    timer.tick(cycles);
    apu.tick(cycles);
    ppu.tick(cycles);
}
