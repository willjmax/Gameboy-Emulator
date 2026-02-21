#pragma once
#include "instruction.h"
#include "memory.h"

#define REG_PAIR(hi, lo, combined) \
    union {                        \
        struct {                   \
            uint8_t lo;            \
            uint8_t hi;            \
        };                         \
        uint16_t combined;         \
    }

class CPU {
    public:
        CPU(Bus& b) : memory(b), pc(0x100), sp(0xFFFE) {};

        void step();
        uint8_t fetch();
        uint8_t execute(Instruction instr);

        uint8_t getZ();
        uint8_t getN();
        uint8_t getH();
        uint8_t getC();

        void setZ(uint8_t flag);
        void setN(uint8_t flag);
        void setH(uint8_t flag);
        void setC(uint8_t flag);

        uint8_t get_reg(uint8_t reg);
        void set_reg(uint8_t reg, uint8_t byte);

        uint16_t get_pair(uint8_t pair);
        void set_pair(uint8_t pair, uint16_t bytes);

    private:
        Bus& memory;
        REG_PAIR(a, f, af);
        REG_PAIR(b, c, bc);
        REG_PAIR(d, e, de);
        REG_PAIR(h, l, hl);
        uint16_t pc;
        uint16_t sp;

        uint8_t execute_block_00(Instruction instr);
        uint8_t execute_block_01(Instruction instr);
        uint8_t execute_block_10(Instruction instr);
        uint8_t execute_block_11(Instruction instr);

        uint8_t load_from_r8(uint8_t r8);
};

#undef REG_PAIR
