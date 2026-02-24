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
        CPU(Bus& b) : memory(b), pc(0x100), sp(0xFFFE), halted(false) {};
        void step();

    private:
        Bus& memory;
        REG_PAIR(a, f, af);
        REG_PAIR(b, c, bc);
        REG_PAIR(d, e, de);
        REG_PAIR(h, l, hl);
        uint16_t pc;
        uint16_t sp;
        bool halted;
        bool ime;
        bool ei;

        // fetch bytes
        uint8_t fetch();
        uint16_t fetch_two_bytes();

        // stack ops
        void push(uint8_t data);
        uint8_t pop();

        // toggle interrupts
        void ime_on();
        void ime_off();

        // execute instruction
        uint8_t execute(Instruction instr);
        uint8_t execute_block_00(Instruction instr);
        uint8_t execute_block_01(Instruction instr);
        uint8_t execute_block_10(Instruction instr);
        uint8_t execute_block_11(Instruction instr);

        // read/write from register
        uint8_t read_r8(uint8_t r8);
        uint16_t read_r16(uint8_t r16);
        uint16_t read_r16_stack(uint8_t r16);
        uint16_t read_r16_mem(uint8_t r16);
        void write_r8(uint8_t r8, uint8_t data);
        void write_r16(uint8_t r16, uint16_t data);
        void write_r16_stack(uint8_t r16, uint16_t data);
        void write_r16_mem(uint8_t r16, uint16_t data);

        // get/set flags
        uint8_t getZ();
        uint8_t getN();
        uint8_t getH();
        uint8_t getC();
        void setZ(uint8_t flag);
        void setN(uint8_t flag);
        void setH(uint8_t flag);
        void setC(uint8_t flag);
        bool cond(uint8_t cond);

        // helper class for testing
        friend class CPUTester;
};

#undef REG_PAIR
