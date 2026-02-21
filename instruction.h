#pragma once
#include <cstdint>

class Instruction {
    public:
        Instruction(uint8_t opcode);
        uint8_t range(uint8_t start, uint8_t end);

        uint8_t block;
        uint8_t opcode;
};
