#include "cpu.h"

void CPU::step() {

}

void CPU::setZ(bool bit) {
    if (bit) {
        f |= (1 << 7);
    } else {
        f &= (1 << 7);
    }
}

void CPU::setN(bool bit) {
    if (bit) {
        f |= (1 << 6);
    } else {
        f &= (1 << 6);
    }
}

void CPU::setH(bool bit) {
    if (bit) {
        f |= (1 << 5);
    } else {
        f &= (1 << 5);
    }
}

void CPU::setC(bool bit) {
    if (bit) {
        f |= (1 << 4);
    } else {
        f &= (1 << 4);
    }
}

bool CPU::getZ() {
    return (f & (1 << 7)) != 0;
}

bool CPU::getN() {
    return (f & (1 << 6)) != 0;
}

bool CPU::getH() {
    return (f & (1 << 5)) != 0;
}

bool CPU::getC() {
    return (f & (1 << 4)) != 0;
}
