#include <stdexcept>
#include "cpu.h"

void CPU::step() {
    uint8_t opcode = fetch();
    Instruction instr(opcode);
    execute(instr);
}

void CPU::handle_interrupt() {
    uint16_t interrupt_loc = interrupt.check_interrupts();
    if (interrupt_loc > 0) {
        bus.tick(8);
        push2(pc);
        pc = interrupt_loc;
        bus.tick(4);
    }
}

uint8_t CPU::fetch() {
    uint8_t opcode = bus.read(pc);
    pc++;

    return opcode;
}

uint16_t CPU::fetch_two_bytes() {
    uint8_t byte1 = bus.read(pc);        
    pc++;

    uint8_t byte2 = bus.read(pc);
    pc++;

    return (byte2 << 8) | byte1;
}

void CPU::push(uint8_t data) {
    sp--;
    bus.write(sp, data);
}

void CPU::push2(uint16_t data) {
    uint8_t lo = data & 0x00FF;
    uint8_t hi = data >> 8;

    sp--;
    bus.write(sp, hi);

    sp--;
    bus.write(sp, lo);
}

uint8_t CPU::pop() {
    uint8_t data = bus.read(sp);
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
            return bus.read(hl);
        case 7:
            return a;
    }

    return 0;
}

void CPU::write_r8(uint8_t r8, uint8_t data) {
    switch (r8) {
        case 0:
            b = data;
            break;
        case 1:
            c = data;
            break;
        case 2:
            d = data; 
            break;
        case 3:
            e = data;
            break;
        case 4:
            h = data; 
            break;
        case 5:
            l = data;
            break;
        case 6:
            bus.write(hl, data);
            break;
        case 7:
            a = data;
            break;
    }
}

uint16_t CPU::read_r16(uint8_t r16) {
    switch (r16) {
        case 0:
            return bc;
        case 1:
            return de;
        case 2:
            return hl;
        case 3:
            return sp;
        default:
            throw std::runtime_error("read_r16: Invalid register given");
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

uint16_t CPU::read_r16_stack(uint8_t r16) {
    switch (r16) {
        case 0:
            return bc;
        case 1:
            return de;
        case 2:
            return hl;
        case 3:
            return af;
        default:
            throw std::runtime_error("read_r16_stack: Invalid register given");
    }
}

void CPU::write_r16_stack(uint8_t r16, uint16_t data) {
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
            af = data & 0xFFF0;
            break;
        default:
            throw std::runtime_error("write_r16_stack: Invalid register given");
    }
}

uint16_t CPU::read_r16_mem(uint8_t r16) {
    switch (r16) {
        case 0:
            return bus.read(bc);
        case 1:
            return bus.read(de);
        case 2: {
            uint16_t data = bus.read(hl);
            hl++;
            return data;
        }
        case 3: {
            uint16_t data = bus.read(hl);
            hl--;
            return data;
        }
        default:
            throw std::runtime_error("read_16_mem: Invalid register given");
    }
}

void CPU::write_r16_mem(uint8_t r16, uint16_t data) {
    switch (r16) {
        case 0:
            bus.write(bc, data);
            break;
        case 1:
            bus.write(de, data);
            break;
        case 2:
            bus.write(hl, data); 
            hl++;
            break;
        case 3:
            bus.write(hl, data);
            hl--;
            break;
    }
}

bool CPU::cond(uint8_t cond) {
    switch (cond) {
        case 0:
            return getZ() == 0;
        case 1:
            return getZ() != 0;
        case 2:
            return getC() == 0;
        case 3:
            return getC() != 0;
        default:
            throw std::runtime_error("cond: Invalid condition given.");
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
