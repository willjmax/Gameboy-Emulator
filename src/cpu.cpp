#include <stdexcept>
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

uint16_t CPU::fetch_two_bytes() {
    uint8_t byte1 = memory.read(pc);        
    pc++;

    uint8_t byte2 = memory.read(pc);
    pc++;

    return (byte2 << 8) | byte1;
}

void CPU::push(uint8_t data) {
    memory.write(sp, data);
    sp--;
}

uint8_t CPU::pop() {
    uint8_t data = memory.read(sp);
    sp++;
    return data;
}

uint8_t CPU::read_r8(uint8_t r8) {

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

void CPU::write_r8(uint8_t r8, uint8_t data) {
    switch (r8) {
        case 0:
            b = data;
        case 1:
            c = data;
        case 2:
            d = data; 
        case 3:
            e = data;
        case 4:
            h = data; 
        case 5:
            l = data;
        case 6:
            memory.write(hl, data);
        case 7:
            a = data;
    }
}

void CPU::write_r16(uint8_t r16, uint16_t data) {
    switch (r16) {
        case 0:
            bc = data;
            break;
        case 1:
            de = data;
            break;
        case 2:
            hl = data;
            break;
        case 3:
            sp = data;
            break;
        default:
            throw std::runtime_error("write_r16: Invalid register given");
    }
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

void CPU::ime_on() {
    ime = 1;
}

void CPU::ime_off() {
    ime = 0;
}

