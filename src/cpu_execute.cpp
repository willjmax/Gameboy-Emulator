#include "cpu.h"

bool half_carry(uint16_t a, uint16_t b) {
    return (a & 0x0F) + (b & 0x0F) > 0x0F;
}

bool half_carry2(uint16_t a, uint16_t b, uint16_t c) {
    return (a & 0x0F) + (b & 0x0F) + (c & 0x0F) > 0x0F;
}

bool half_borrow(uint16_t a, uint16_t b) {
    return (b & 0x0F) > (a & 0x0F);
}

bool half_borrow2(uint16_t a, uint16_t b, uint16_t c) {
    return (b & 0x0F) + c > (a & 0x0F);
}

void CPU::execute(Instruction instr) {

    switch (instr.block) {
        case 0b00:
            execute_block_00(instr);
            break;
        case 0b01:
            execute_block_01(instr);
            break;
        case 0b10:
            execute_block_10(instr);
            break;
        case 0b11:
            execute_block_11(instr);
            break;
    }

}

void CPU::execute_block_00(Instruction instr) {

    switch (instr.opcode) {

        // NOP
        case 0x00: {
            return;
        }

        // RLCA
        case 0x07: {
            uint8_t new_carry = a >> 7;
            uint8_t shifted = (a << 1) | new_carry;

            a = shifted;

            setZ(0);
            setN(0);
            setH(0);
            setC(new_carry);
            return;
        }

        // RRCA
        case 0x0F: {
            uint8_t new_carry = a & 0x1;
            a = (a >> 1) | (new_carry << 7);
            setZ(0);
            setN(0);
            setH(0);
            setC(new_carry);
            return;
        }

        // STOP
        case 0x10: {
            timer.enter_stop_mode();
            timer.reset_div();
            return;
        }
        
        // JR imm8
        case 0x18: {
            int8_t offset = (int8_t)fetch();
            pc += offset;
            bus.tick(4);
            return;
        }

        // RLA
        case 0x17: {
            uint8_t new_carry = a >> 7;
            uint8_t shifted = (a << 1) | getC();
            a = shifted;
            setZ(0);
            setN(0);
            setH(0);
            setC(new_carry);
            return;
        }

        // RRA
        case 0x1F: {
            uint8_t new_carry = a & 0x01;
            uint8_t shifted = (a >> 1) | (getC() << 7);
            a = shifted;
            setZ(0);
            setN(0);
            setH(0);
            setC(new_carry);
            return;
        }

        // DAA
        case 0x27: {
            uint8_t adjustment = 0;

            if (getN()) {
                if (getH()) {
                    adjustment += 0x6;
                }

                if (getC()) {
                    adjustment += 0x60;
                }

                a -= adjustment;
            } else {
                if (getH() || ((a & 0xF) > 0x9)) {
                    adjustment += 6;
                }

                if (getC() || (a > 0x99)) {
                    adjustment += 0x60;
                    setC(1);
                }

                a += adjustment;
            }

            setZ(a == 0);
            setH(0);

            return;
        }

        // CPL
        case 0x2F: {
            a = ~a;
            setN(1);
            setH(1);
            return;
        }

        // SCF
        case 0x37: {
            setN(0);
            setH(0);
            setC(1);
            return;
        }

        // CCF
        case 0x3F: {
            setN(0);
            setH(0);
            setC(getC() == 0);
            return;
        }

    }
    
    // JR cond imm8
    if (instr.range(5, 5) == 1 && instr.range(2, 0) == 0) {
        uint8_t data = instr.range(4, 3);
        int8_t offset = (int8_t)fetch();
        if (cond(data)) {
            pc += offset;
            bus.tick(4);
        }
        return;
    }

    switch (instr.range(3, 0)) {

        // LD r16,imm16
        case 0b0001: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = fetch_two_bytes();
            write_r16(r16, data);
            return;
        }

        // LD [r16mem],a
        case 0b0010: {
            uint8_t r16 = instr.range(5, 4);
            write_r16_mem(r16, a);
            return;
        }

        // INC r16
        case 0b0011: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = read_r16(r16);
            write_r16(r16, data + 1);
            bus.tick(4);
            return;
        }

        // LD [imm16],sp
        case 0b1000: {
            uint16_t loc = fetch_two_bytes();
            bus.write(loc, sp & 0xFF);
            bus.write(loc + 1, sp >> 8);
            return;
        } 
                     
        // ADD HL,r16
        case 0b1001: {
            uint8_t r16 = instr.range(5, 4);
            uint32_t data = read_r16(r16);
            uint32_t result = hl + data;
            setN(0);
            setH((hl & 0x0FFF) + (data & 0x0FFF) > 0x0FFF);
            setC(result > 0x0000FFFF);

            hl = result & 0x0000FFFF;
            bus.tick(4);
            return;
        }

        // LD A,[r16mem]
        case 0b1010: {
            uint16_t r16 = instr.range(5, 4);
            a = read_r16_mem(r16);
            return;
        }

        // DEC r16
        case 0b1011: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = read_r16(r16);
            write_r16(r16, data - 1);
            bus.tick(4);
            return;
        }
    }

    switch (instr.range(2, 0)) {

        // INC r8
        case 0b100: {
            uint8_t r8 = instr.range(5, 3);
            uint8_t data = read_r8(r8);
            uint8_t result = data + 1;

            write_r8(r8, result);

            setZ(result == 0);
            setN(0);
            setH(half_carry(data, 1));
            return;
        }

        // DEC r8
        case 0b101: {
            uint8_t r8 = instr.range(5, 3);
            uint8_t data = read_r8(r8);
            uint8_t result = data - 1;

            write_r8(r8, result);

            setZ(result == 0);
            setN(1);
            setH(half_borrow(data, 1));
            return;
        }

        // LD r8,imm8
        case 0b110: {
            uint8_t r8 = instr.range(5, 3);
            uint8_t data = fetch();
            write_r8(r8, data);
            return;
        }
    }

}

void CPU::execute_block_01(Instruction instr) {

    // HALT
    if (instr.opcode == 0x76) {
        interrupt.halt_cpu();
        return;
    }

    uint8_t src = instr.range(2, 0);
    uint8_t dest = instr.range(5, 3);
    uint8_t data = read_r8(src);

    write_r8(dest, data);

    return;
}

void CPU::execute_block_10(Instruction instr) {
    uint8_t op = instr.range(5, 3);
    uint8_t r8 = instr.range(2, 0);

    switch (op) {

        // ADD A,r8
        case 0b000: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t result = a16 + val;

            setZ((result & 0xFF) == 0);
            setN(0);
            setH(half_carry(a16, val));
            setC(result > 0xFF);

            a = result & 0xFF;
            return;
        }

        // ADC A,r8
        case 0b001: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 + val + carry;

            setZ((result & 0xFF) == 0);
            setN(0);
            setH(half_carry2(a16, val, carry));
            setC(result > 0xFF);

            a = result & 0xFF;
            return;
        }

        // SUB A,r8
        case 0b010: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t result = a16 - val;

            setZ((result & 0xFF) == 0);
            setN(1);
            setH(half_borrow(a16, val));
            setC(val > a16);

            a = result & 0xFF;
            return;
        }

        // SBC A,r8
        case 0b011: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 - val - carry;

            setZ((result & 0xFF) == 0);
            setN(1);
            setH(half_borrow2(a16, val, carry));
            setC((val + carry) > a16);

            a = result & 0xFF;
            return;
        }

        // AND A,r8
        case 0b100: {
            uint8_t val = read_r8(r8);
            uint8_t result = a & val;

            setZ(result == 0);
            setN(0);
            setH(1);
            setC(0);
            a = result;
            return;
        }

        // XOR A,r8
        case 0b101: {
            uint8_t val = read_r8(r8);
            uint8_t result = a ^ val;

            setZ(result == 0);
            setN(0);
            setH(0);
            setC(0);

            a = result;
            return;
        }

        // OR A,r8
        case 0b110: {
            uint8_t val = read_r8(r8);
            uint8_t result = a | val;

            setZ(result == 0);
            setN(0);
            setH(0);
            setC(0);

            a = result;
            return;
        }

        // CP A,r8
        case 0b111: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t result = a16 - val;

            setZ((result & 0xFF) == 0);
            setN(1);
            setH(half_borrow(a16, val));
            setC(val > a16);

            return;
        }
    }
}


void CPU::execute_block_11(Instruction instr) {

    switch (instr.opcode) {

        // JP imm16
        case 0xC3: {
            pc = fetch_two_bytes();
            bus.tick(4);
            return;
        }

        // ADD A,imm8
        case 0xC6: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = fetch();
            uint16_t result = a16 + val;

            setZ((result & 0xFF) == 0);
            setN(0);
            setH(half_carry(a16, val));
            setC(result > 0xFF);

            a = result & 0xFF;
            return;
        }

        // ADC a,imm8
        case 0xCE: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = fetch();
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 + val + carry;

            setZ((result & 0xFF) == 0);
            setN(0);
            setH(half_carry2(a16, val, carry));
            setC(result > 0xFF);

            a = result & 0xFF;
            return;
       }

        // RET
        case 0xC9: {
            uint8_t lo = pop();
            uint8_t hi = pop();
            pc = (hi << 8) | lo;
            bus.tick(4);
            return;
        }

        // CB
        case 0xCB: {
            uint8_t cb_instr = fetch();
            execute_cb(cb_instr);
            return;
        }

        // CALL imm16
        case 0xCD: {
            uint16_t loc = fetch_two_bytes();
            bus.tick(4);
            push((pc >> 8) & 0xFF);
            push(pc & 0xFF);
            pc = loc;
            return;
        }

        // SUB a,imm8
        case 0xD6: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = fetch();
            uint16_t result = a16 - val;
            setZ((result & 0xFF) == 0);
            setN(1);
            setH(half_borrow(a16, val));
            setC(val > a16);
            a = result & 0xFF;
            return;
        }

        // RETI
        case 0xD9: {
            interrupt.set_ime();
            uint8_t lo = pop();
            uint8_t hi = pop();
            pc = (hi << 8) | lo;
            bus.tick(4);
            return;
        }

        // SBC a,imm8
        case 0xDE: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = fetch();
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 - val - carry;
            setZ((result & 0xFF) == 0);
            setN(1);
            setH(half_borrow2(a16, val, carry));
            setC((val + carry) > a16);
            a = result & 0xFF;
            return;
        }

        // LDH [imm8],A
        case 0xE0: {
            uint8_t offset = fetch();
            uint16_t loc = 0xFF00 | (uint16_t)offset;
            bus.write(loc, a);
            return;
        }

        // LDH [C],A
        case 0xE2: {
            uint16_t loc = 0xFF00 | c;
            bus.write(loc, a);
            return;
        }

        // AND A,imm8
        case 0xE6: {
            uint8_t data = fetch();
            uint8_t result = a & data;
            setZ(result == 0);
            setN(0);
            setH(1);
            setC(0);
            a = result;
            return;
        }

        // ADD SP,imm8
        case 0xE8: {
            int8_t offset = (int8_t)fetch();
            uint16_t result = sp + offset;
            setZ(0);
            setN(0);
            setH((sp & 0x000F) + (offset & 0x0F) > 0x0F);
            setC((sp & 0x00FF) + (offset & 0x00FF) > 0xFF);
            sp = result;
            bus.tick(4);
            bus.tick(4);
            return;
        }

        // JP HL
        case 0xE9: {
            pc = hl;
            return;
        }

        // LD [imm16],A
        case 0xEA: {
            uint16_t loc = fetch_two_bytes();
            bus.write(loc, a);
            return;
        }

        // XOR a,imm8
        case 0xEE: {
            uint8_t val = fetch();
            uint8_t result = a ^ val;

            setZ(result == 0);
            setN(0);
            setH(0);
            setC(0);

            a = result;
            return;
        }

        // LDH A,[imm8]
        case 0xF0: {
            uint8_t lo = fetch();
            uint16_t loc = 0xFF00 | (uint16_t)lo;
            a = bus.read(loc);
            return;
        }

        // LDH A,[C]
        case 0xF2: {
            uint16_t loc = 0xFF00 | c;
            a = bus.read(loc);
            return;
        }

        // LD A,[imm16]
        case 0xFA: {
            uint16_t loc = fetch_two_bytes();
            a = bus.read(loc);
            return;
        }

        // DI
        case 0xF3: {
            interrupt.clear_ime();
            return;
        }

        // OR a,imm8
        case 0xF6: {
            uint8_t val = fetch();
            uint8_t result = a | val;
            setZ(result == 0);
            setN(0);
            setH(0);
            setC(0);
            a = result;
            return;
        }

        // LD HL, sp+imm8
        case 0xF8: {
            int8_t offset = (int8_t)fetch();
            uint16_t result = sp + offset;

            setZ(0);
            setN(0);
            setH((sp & 0x000F) + (offset & 0x0F) > 0x0F);
            setC((sp & 0x00FF) + (offset & 0x00FF) > 0xFF);

            hl = result;
            bus.tick(4);
            return;
        }

        // LD SP,HL
        case 0xF9: {
            sp = hl;
            bus.tick(4);
            return;
        }

        // EI
        case 0xFB: {
            interrupt.set_ime_delay();
            return;
        }

        // CP A,imm8
        case 0xFE: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = fetch();
            uint16_t result = a16 - val;

            setZ((result & 0xFF) == 0);
            setN(1);
            setH(half_borrow(a16, val));
            setC(val > a16);
            return;
        }
    }

    switch (instr.range(3, 0)) {

        // POP
        case 0b0001: {
            uint8_t r16 = instr.range(5, 4);
            uint8_t lo = pop();
            uint8_t hi = pop();
            uint16_t data = (hi << 8) | lo;
            write_r16_stack(r16, data);
            return;
        }

        // PUSH
        case 0b0101: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = read_r16_stack(r16);
            uint8_t lo = data & 0x00FF;
            uint8_t hi = data >> 8;
            bus.tick(4);
            push(hi);
            push(lo);
            return;
        }
    }

    if (instr.range(5, 5) == 0) {
        switch (instr.range(2, 0)) {

            // RET cond
            case 0b000: {
                uint8_t data = instr.range(4, 3);
                bus.tick(4);

                if (cond(data)) {
                    uint8_t lo = pop();
                    uint8_t hi = pop();
                    uint16_t loc = (hi << 8) | lo;
                    pc = loc;
                    bus.tick(4);
                }

                return;
            }

            // JP cond,imm16
            case 0b010: {
                uint8_t data = instr.range(4, 3);
                uint16_t loc = fetch_two_bytes();


                if (cond(data)) {
                    bus.tick(4);
                    pc = loc;
                    return;
                }

                return;
            }

            // CALL cond,imm16
            case 0b100: {
                uint8_t data = instr.range(4, 3);
                uint16_t loc = fetch_two_bytes();

                if (cond(data)) {
                    bus.tick(4);
                    push((pc >> 8) & 0xFF);
                    push(pc & 0xFF);
                    pc = loc;
                }

                return;
            }

            case 0b110:
                throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }
    }

    // RST
    if (instr.range(2, 0) == 7) {
        uint16_t target = instr.range(5, 3) << 3;
        uint8_t hi = pc >> 8;
        uint8_t lo = pc & 0xFF;
        push(hi);
        push(lo);
        pc = target;
        bus.tick(4);
        return;
    }

    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));

}

void CPU::execute_cb(Instruction instr) {

    if (instr.block == 0b00) {
        uint8_t r8 = instr.range(2, 0);
        uint8_t data = read_r8(r8);

        switch (instr.range(5, 3)) {

            // RLC r8
            case 0: {
                setC(data >> 7);
                uint8_t shifted = (data << 1) | getC();

                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);

                return;
            }

            // RRC r8
            case 1: {
                setC(data & 0x01);
                uint8_t shifted = (data >> 1) | (getC() << 7);

                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);
            
                return;
            }

            // RL r8
            case 2: {
                uint8_t new_carry = data >> 7;
                uint8_t shifted = (data << 1) | getC();

                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);
                setC(new_carry);

                return;
            }

            // RR r8
            case 3: {
                uint8_t new_carry = data & 0x01;
                uint8_t shifted = (data >> 1) | (getC() << 7);

                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);
                setC(new_carry);

                return;
            }

            // SLA r8
            case 4: {
                uint8_t new_carry = data >> 7;
                uint8_t shifted = data << 1;

                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);
                setC(new_carry);

                return;
            }

            // SRA r8
            case 5: {
                uint8_t new_carry = data & 0x01;
                uint8_t shifted = (data >> 1) | (data & 0x80);
                
                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);
                setC(new_carry);

                return;
            }

            // SWAP r8
            case 6: {
                uint8_t swapped = (data >> 4) | (data << 4);

                write_r8(r8, swapped);
                setZ(swapped == 0);
                setN(0);
                setH(0);
                setC(0);

                return;
            }

            // SRL r8
            case 7: {
                uint8_t new_carry = data & 0x01;
                uint8_t shifted = data >> 1;

                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);
                setC(new_carry);

                return;
            }
        }
    }

    uint8_t b3 = instr.range(5, 3);
    uint8_t r8 = instr.range(2, 0);
    uint8_t data = read_r8(r8);

    switch (instr.block) {

        // BIT b3,r8
        case 0b01: {
            uint8_t is_set = (data >> b3) & 0x01;

            setZ(is_set == 0);
            setN(0);
            setH(1);

            return;
        }

        //RES b3,r8
        case 0b10: {
            uint8_t off = data & ~(1 << b3);
            write_r8(r8, off);

            return;
        }

        // SET b3,r8
        case 0b11: {
            uint8_t on = data | (1 << b3);
            write_r8(r8, on);

            return;
        }
    }

}
