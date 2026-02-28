#include <cstdint>
#include <iostream>
#include "memory.h"

uint8_t Bus::read(uint16_t loc) {

    if (loc == Bus::IF_REG) {
        return Bus::memory[loc] | 0xE0;
    }

    return Bus::memory[loc];
}

void Bus::write(uint16_t loc, uint8_t byte) {

    switch (loc) {
        case TERMINAL:
            std::cout << (char)byte << std::flush;
            Bus::memory[loc] = byte;
            break;
        case DIV:
            Bus::memory[loc] = 0x00;
            break;
        default:
            Bus::memory[loc] = byte;
            break;
    }

}

void Bus::loadROM(std::ifstream& file, uintmax_t size) {
    file.read(reinterpret_cast<char*>(&memory[Bus::ROM_FIXED_START]), size);
}

void Bus::update_div(uint8_t data) {
    Bus::memory[Bus::DIV] = data;
}
