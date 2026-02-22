#include <format>
#include <stdexcept>
#include "cpu.h"

#include <iostream>

bool half_carry(uint16_t a, uint16_t b) {
    return (a & 0x0F) + (b & 0x0F) > 0x0F;
}

bool half_borrow(uint16_t a, uint16_t b) {
    return (b & 0x0F) > (a & 0x0F);
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

    switch (instr.range(3, 0)) {

        // LD r16,imm16
        case 0b0001: {
            uint8_t r16 = instr.range(5, 4);
            uint16_t data = fetch_two_bytes();
            write_r16(r16, data);
            return 3;
        }

        case 0b0010:
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        case 0b1010:
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
        case 0b1000:
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
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
            setH(half_borrow(r8, 1));

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
    return 0;
}

uint8_t CPU::execute_block_01(Instruction instr) {
    uint8_t op = instr.range(5, 3);
    uint8_t r8 = instr.range(2, 0);

    switch (op) {

        // ADD A,r8
        case 0b000: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t result = a16 + val;

            setZ((result & 0x0F) == 0);
            setN(0);
            setH(half_carry(a16, val));
            setC(result > 0xFF);

            a = result & 0x0F;
            break;
        }

        // ADC A,r8
        case 0b001: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 + val + carry;

            setZ((result & 0x0F) == 0);
            setN(0);
            setH(half_carry(a16, val + carry));
            setC(result > 0xFF);

            a = result & 0x0F;
            break;
        }

        // SUB A,r8
        case 0b010: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t result = a16 - val;

            setZ((result & 0x0F) == 0);
            setN(1);
            setH(half_borrow(a16, val));
            setC(val > a16);

            a = result & 0x0F;
            break;
        }

        // SBC A,r8
        case 0b011: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 - val - carry;

            setZ((result & 0x0F) == 0);
            setN(1);
            setH(half_borrow(a16, val + carry));
            setC((val + carry) > a16);

            a = result & 0x0F;
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

        // CP A,8
        case 0b111: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = read_r8(r8);
            uint16_t result = a16 - val;

            setZ((result & 0x0F) == 0);
            setN(1);
            setH(half_carry(a16, val));
            setC(val > a16);

            break;
        }
    }

    if (r8 == 6) {
        return 2;
    }

    return 1;
}

uint8_t CPU::execute_block_10(Instruction instr) {
    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
    return 0;
}

uint8_t CPU::execute_block_11(Instruction instr) {

    if (instr.range(2, 0) == 0b110) {
        throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
    }

    switch (instr.opcode) {

        // JP nn
        case 0xC3: {
            pc = fetch_two_bytes();
            return 4;
        }

        // RET
        case 0xC9: {
            pc = pop();
            return 4;
        }

        // CALL [imm16]
        case 0xCD: {
            push(pc);
            pc = fetch_two_bytes();
            return 6;
        }

        // LDH [imm8],A
        case 0xE0: {
            uint8_t offset = fetch();
            uint16_t loc = 0xFF00 | (uint16_t)offset;
            std::cout << std::format("0x{:02X}", offset) << std::endl;
            std::cout << std::format("Writing to 0x{:02X}", loc) << std::endl;
            memory.write(loc, a);
            return 3;
        }

        // LDH [C],A
        case 0xE2:
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));

        // LD [imm16],A
        case 0xEA: {
            uint16_t loc = fetch_two_bytes();
            memory.write(loc, a);
            return 4;
        }

        // LDH A,[imm8]
        case 0xF0:
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));

        // LDH A,[C]
        case 0xF2:
            throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));

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

        // EI
        case 0xFB: {
            ime_on();
            return 4;
        }
    }

    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
}
