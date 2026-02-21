#include "cpu.h"

void CPU::step() {
    uint8_t opcode = fetch();
    Instruction instr(opcode);
    execute(instr);
}

uint8_t CPU::fetch() {
    uint8_t opcode = memory.read(pc);
    pc++;

    return opcode;
}

uint8_t CPU::load_from_r8(uint8_t r8) {

    switch (r8) {
        case 0:
            return b;
        case 1:
            return c;
        case 2:
            return d; 
        case 3:
            return e;
        case 4:
            return h; 
        case 5:
            return l ;
        case 6:
            return memory.read(hl);
        case 7:
            return a;
    }

    return 0;
}

void CPU::setZ(uint8_t flag) {
    if (flag) {
        f |= (1 << 7);
    } else {
        f &= 0x7F;
    }
}

void CPU::setN(uint8_t flag) {
    if (flag > 0) {
        f |= (1 << 6);
    } else {
        f &= 0xBF;
    }
}

void CPU::setH(uint8_t flag) {
    if (flag > 0) {
        f |= (1 << 5);
    } else {
        f &= 0xDF;
    }
}

void CPU::setC(uint8_t flag) {
    if (flag > 0) {
        f |= (1 << 4);
    } else {
        f &= 0xEF;
    }
}

uint8_t CPU::getZ() {
    return (f & (1 << 7)) != 0;
}

uint8_t CPU::getN() {
    return (f & (1 << 6)) != 0;
}

uint8_t CPU::getH() {
    return (f & (1 << 5)) != 0;
}

uint8_t CPU::getC() {
    return (f & (1 << 4)) != 0;
}
