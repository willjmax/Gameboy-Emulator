#include "cpu.h"
#include <iostream>

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
            return execute_block_00(instr);
        case 0b01:
            return execute_block_01(instr);
        case 0b10:
            return execute_block_10(instr);
        case 0b11:
            return execute_block_11(instr);
    }

    return 1;
}

uint8_t CPU::execute_block_00(Instruction instr) {

    switch (instr.opcode) {

        // NOP
        case 0x00: {
            return 1;
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
            
            return 1;
        }

        // RRCA
        case 0x0F: {
            uint8_t new_carry = a & 0x1;
            a = (a >> 1) | (new_carry << 7);
            setZ(0);
            setN(0);
            setH(0);
            setC(new_carry);
            return 1;
        }

        // STOP
        case 0x10: {
            timer.enter_stop_mode();
            timer.reset_div();
            return 0;
        }
        
        // JR imm8
        case 0x18: {
            int8_t offset = (int8_t)fetch();
            pc += offset;
            return 3;
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

            return 1;
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

            return 1;
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

            return 1;
        }

        // CPL
        case 0x2F: {
            a = ~a;
            setN(1);
            setH(1);
            return 1;
        }

        // SCF
        case 0x37: {
            setN(0);
            setH(0);
            setC(1);
            return 1;
        }

        // CCF
        case 0x3F: {
            setN(0);
            setH(0);
            setC(getC() == 0);
            return 1;
        }

    }
    
    // JR cond imm8
    if (instr.range(5, 5) == 1 && instr.range(2, 0) == 0) {
        uint8_t data = instr.range(4, 3);
        int8_t offset = (int8_t)fetch();
        if (cond(data)) {
            pc += offset;
            return 3;
        }
        return 2;
    }

    switch (instr.range(3, 0)) {

        // LD r16,imm16
        case 0b0001: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = fetch_two_bytes();
            write_r16(r16, data);
            return 3;
        }

        // LD [r16mem],a
        case 0b0010: {
            uint8_t r16 = instr.range(5, 4);
            write_r16_mem(r16, a);
            return 3;
        }

        // INC r16
        case 0b0011: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = read_r16(r16);
            write_r16(r16, data + 1);
            return 2;
        }

        // LD [imm16],sp
        case 0b1000: {
            uint16_t loc = fetch_two_bytes();
            memory.write(loc, sp & 0xFF);
            memory.write(loc + 1, sp >> 8);
            return 3;
        } 
                     
        // ADD hl,r16
        case 0b1001: {
            uint8_t r16 = instr.range(5, 4);
            uint32_t data = read_r16(r16);
            uint32_t result = hl + data;
            setN(0);
            setH((hl & 0x0FFF) + (data & 0x0FFF) > 0x0FFF);
            setC(result > 0x0000FFFF);

            hl = result & 0x0000FFFF;
            return 2;
        }

        // LD A,[r16mem]
        case 0b1010: {
            uint16_t r16 = instr.range(5, 4);
            a = read_r16_mem(r16);
            return 2;
        }

        // DEC r16
        case 0b1011: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = read_r16(r16);
            write_r16(r16, data - 1);
            return 2;
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

            return 1;
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

            return 1;
        }

        // LD r8,imm8
        case 0b110: {
            uint8_t r8 = instr.range(5, 3);
            uint8_t data = fetch();
            write_r8(r8, data);
            return 2;
        }
    }


    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
}

uint8_t CPU::execute_block_01(Instruction instr) {

    // HALT
    if (instr.opcode == 0x76) {
        interrupts.halt_cpu();
        return 0;
    }

    uint8_t src = instr.range(2, 0);
    uint8_t dest = instr.range(5, 3);
    uint8_t data = read_r8(src);

    write_r8(dest, data);

    return 1;
}

uint8_t CPU::execute_block_10(Instruction instr) {
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
            break;
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
            break;
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
            break;
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
            break;
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
            break;
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
            break;
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
            break;
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

            break;
        }
    }

    if (r8 == 6) {
        return 2;
    }

    return 1;
}


uint8_t CPU::execute_block_11(Instruction instr) {

    switch (instr.opcode) {

        // JP imm16
        case 0xC3: {
            pc = fetch_two_bytes();
            return 4;
        }

        // ADD a,imm8
        case 0xC6: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = fetch();
            uint16_t result = a16 + val;

            setZ((result & 0xFF) == 0);
            setN(0);
            setH(half_carry(a16, val));
            setC(result > 0xFF);

            a = result & 0xFF;
            return 2;
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
            return 1;
       }

        // RET
        case 0xC9: {
            uint8_t lo = pop();
            uint8_t hi = pop();
            pc = (hi << 8) | lo;
            return 4;
        }

        // CB
        case 0xCB: {
            uint8_t cb_instr = fetch();
            return execute_cb(cb_instr);
        }

        // CALL imm16
        case 0xCD: {
            uint16_t loc = fetch_two_bytes();
            push((pc >> 8) & 0xFF);
            push(pc & 0xFF);
            pc = loc;
            return 6;
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

            return 2;
        }

        // RETI
        case 0xD9: {
            interrupts.set_ime();
            uint8_t lo = pop();
            uint8_t hi = pop();
            pc = (hi << 8) | lo;
            return 4;
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

            return 2;
        }

        // LDH [imm8],A
        case 0xE0: {
            uint8_t offset = fetch();
            uint16_t loc = 0xFF00 | (uint16_t)offset;
            memory.write(loc, a);
            return 3;
        }

        // LDH [C],A
        case 0xE2: {
            uint16_t loc = 0xFF00 | c;
            memory.write(loc, a);
            return 2;
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

            return 2;
        }

        // ADD sp,imm8
        case 0xE8: {
            int8_t offset = (int8_t)fetch();
            uint16_t result = sp + offset;

            setZ(0);
            setN(0);
            setH((sp & 0x000F) + (offset & 0x0F) > 0x0F);
            setC((sp & 0x00FF) + (offset & 0x00FF) > 0xFF);

            sp = result;

            return 4;
        }

        // JP HL
        case 0xE9: {
            pc = hl;
            return 1;
        }

        // LD [imm16],A
        case 0xEA: {
            uint16_t loc = fetch_two_bytes();
            memory.write(loc, a);
            return 4;
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
            return 2;
        }

        // LDH A,[imm8]
        case 0xF0: {
            uint8_t lo = fetch();
            uint16_t loc = 0xFF00 | (uint16_t)lo;
            a = memory.read(loc);
            return 3;
        }

        // LDH A,[C]
        case 0xF2: {
            uint16_t loc = 0xFF00 | c;
            a = memory.read(loc);
            return 2;
        }

        // LD A,[imm16]
        case 0xFA: {
            uint16_t loc = fetch_two_bytes();
            a = memory.read(loc);
            return 2;
        }

        // DI
        case 0xF3: {
            interrupts.clear_ime();
            return 4;
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
            return 2;
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
            return 3;
        }

        // LD SP,HL
        case 0xF9: {
            sp = hl;
            return 2;
        }

        // EI
        case 0xFB: {
            interrupts.set_ime_delay();
            return 4;
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
            return 2;
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
            return 3;
        }

        // PUSH
        case 0b0101: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = read_r16_stack(r16);
            uint8_t lo = data & 0x00FF;
            uint8_t hi = data >> 8;
            push(hi);
            push(lo);
            return 4;
        }
    }

    if (instr.range(5, 5) == 0) {
        switch (instr.range(2, 0)) {

            // RET cond
            case 0b000: {
                uint8_t data = instr.range(4, 3);

                if (cond(data)) {
                    uint8_t lo = pop();
                    uint8_t hi = pop();
                    uint16_t loc = (hi << 8) | lo;
                    pc = loc;

                    return 5;
                }

                return 2;
            }

            // JP cond,imm16
            case 0b010: {
                uint8_t data = instr.range(4, 3);
                uint16_t loc = fetch_two_bytes();


                if (cond(data)) {
                    pc = loc;
                    std::cout << "fail" << std::endl;
                    return 4;
                }

                std::cout << "pass" << std::endl;

                return 3;
            }

            // CALL cond,imm16
            case 0b100: {
                uint8_t data = instr.range(4, 3);
                uint16_t loc = fetch_two_bytes();

                if (cond(data)) {
                    push((pc >> 8) & 0xFF);
                    push(pc & 0xFF);
                    pc = loc;
                    return 6;
                }

                return 3;
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

        return 4;
    }

    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
}

uint8_t CPU::execute_cb(Instruction instr) {

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

                return 2;
            }

            // RRC r8
            case 1: {
                setC(data & 0x01);
                uint8_t shifted = (data >> 1) | (getC() << 7);

                write_r8(r8, shifted);
                setZ(shifted == 0);
                setN(0);
                setH(0);
            
                return 2;
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

                return 2;
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

                return 2;
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

                return 2;
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

                return 2;
            }

            // SWAP r8
            case 6: {
                uint8_t swapped = (data >> 4) | (data << 4);

                write_r8(r8, swapped);
                setZ(swapped == 0);
                setN(0);
                setH(0);
                setC(0);

                return 2;
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

                return 2;
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

            return 2;
        }

        //RES b3,r8
        case 0b10: {
            uint8_t off = data & ~(1 << b3);
            write_r8(r8, off);

            return 2;
        }

        // SET b3,r8
        case 0b11: {
            uint8_t on = data | (1 << b3);
            write_r8(r8, on);

            return 2;
        }
    }

    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0xCB 0x{:02X}", instr.opcode));
}
