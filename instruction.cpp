#include "instruction.h"

Instruction::Instruction(uint8_t byte) : opcode(byte) {
    block = range(7, 6);
}

uint8_t Instruction::range(uint8_t start, uint8_t end) {
    uint8_t val = 0x0;

    for (unsigned int i = end; i <= start; ++i) {
        val += opcode & (1 << i);
    }

    return (val >> end);
}
