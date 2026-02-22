#include <cstdint>
#include <iostream>
#include "memory.h"

uint8_t Bus::read(uint16_t loc) {
    return memory[loc];
}

void Bus::write(uint16_t loc, uint8_t byte) {
    if (loc == 0xFF01) {
        std::cout << (char)byte << std::flush;
    }

    memory[loc] = byte;
}

void Bus::loadROM(std::ifstream& file, uintmax_t size) {
    file.read(reinterpret_cast<char*>(&memory[Bus::ROM_FIXED_START]), size);
}
