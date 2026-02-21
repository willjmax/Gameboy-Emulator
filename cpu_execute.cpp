#include <format>
#include <stdexcept>
#include "cpu.h"

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
    if (instr.opcode == 0) {
        return 1; //nop
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
            uint16_t val = load_from_r8(r8);
            uint16_t result = a16 + val;

            setZ((result & 0x0F) == 0);
            setN(0);
            setH((a16 & 0x0F) + (val & 0x0F) > 0x0F);
            setC(result > 0xFF);

            a = result & 0x0F;
            break;
        }

        // ADC A,r8
        case 0b001: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = load_from_r8(r8);
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 + val + carry;

            setZ((result & 0x0F) == 0);
            setN(0);
            setH((a16 & 0x0F) + (val & 0x0F) + (carry & 0x0F) > 0);
            setC(result > 0xFF);

            a = result & 0x0F;
            break;
        }

        // SUB A,r8
        case 0b010: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = load_from_r8(r8);
            uint16_t result = a16 - val;

            setZ((result & 0x0F) == 0);
            setN(1);
            setH((val & 0x0F) > (a16 & 0x0F));
            setC(val > a16);

            a = result & 0x0F;
            break;
        }

        // SBC A,r8
        case 0b011: {
            uint16_t a16 = (uint16_t)a;
            uint16_t val = load_from_r8(r8);
            uint16_t carry = (uint16_t)getC();
            uint16_t result = a16 - val - carry;

            setZ((result & 0x0F) == 0);
            setN(1);
            setH(((val & 0x0F) + (carry & 0x0F)) > (a16 & 0x0F));
            setC((val + carry) > a16);

            a = result & 0x0F;
            break;
        }

        // AND A,r8
        case 0b100: {
            uint8_t val = load_from_r8(r8);
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
            uint8_t val = load_from_r8(r8);
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
            uint8_t val = load_from_r8(r8);
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
            uint16_t val = load_from_r8(r8);
            uint16_t result = a16 - val;

            setZ((result & 0x0F) == 0);
            setN(1);
            setH((val & 0x0F) > (a16 & 0x0F));
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
    throw std::runtime_error("Not implemented: " + std::format("Opcode not implemented: 0x{:02X}", instr.opcode));
    return 0;
}
