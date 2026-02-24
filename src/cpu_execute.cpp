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

uint8_t CPU::execute(Instruction instr) {

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

    // NOP
    if (instr.opcode == 0) {
        return 1;
    }
    
    // JR imm8
    if (instr.opcode == 0x18) {
        int8_t offset = (int8_t)fetch();
        pc += offset;
        return 3;
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
        halted = true;
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
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // ADC a,imm8
        case 0xCE: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
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
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
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
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // RETI
        case 0xD9: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // SBC a,imm8
        case 0xDE: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
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
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // JP HL
        case 0xE9: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // LD [imm16],A
        case 0xEA: {
            uint16_t loc = fetch_two_bytes();
            memory.write(loc, a);
            return 4;
        }

        // XOR a,imm8
        case 0xEE: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // LDH A,[imm8]
        case 0xF0: {
            uint8_t lo = fetch();
            uint16_t loc = 0xFF00 | lo;
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
            ime_off();
            return 4;
        }

        // OR a,imm8
        case 0xF6: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // LD HL, sp+imm8
        case 0xF8: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // LD SP,HL
        case 0xF9: {
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        }

        // EI
        case 0xFB: {
            ime_on();
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

    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
}
