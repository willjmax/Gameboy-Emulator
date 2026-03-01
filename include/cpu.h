#pragma once
#include "instruction.h"
#include "memory.h"
#include "interrupt.h"
#include "timer.h"

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
        CPU(Bus& b) : 
            memory(b), 
            timer(Timer(b)),
            interrupts(Interrupt(b)),
            a(0x01), f(0xB0),
            b(0x00), c(0x13),
            d(0x00), e(0xC1),
            h(0x84), l(0x03),
            pc(0x100), 
            sp(0xFFFE) {};

        void step();

    private:
        Bus& memory;
        Timer timer;
        Interrupt interrupts;

        REG_PAIR(a, f, af);
        REG_PAIR(b, c, bc);
        REG_PAIR(d, e, de);
        REG_PAIR(h, l, hl);

        uint16_t pc;
        uint16_t sp;

        // fetch bytes
        uint8_t fetch();
        uint16_t fetch_two_bytes();

        // bus operations
        uint8_t read_bus(uint16_t loc);
        void write_bus(uint16_t loc, uint8_t data);

        // stack ops
        void push(uint8_t data);
        void push2(uint16_t data);
        uint8_t pop();

        // execute instruction
        void execute(Instruction instr);
        void execute_block_00(Instruction instr);
        void execute_block_01(Instruction instr);
        void execute_block_10(Instruction instr);
        void execute_block_11(Instruction instr);
        void execute_cb(Instruction instr);

        // read from register
        uint8_t read_r8(uint8_t r8);
        uint16_t read_r16(uint8_t r16);
        uint16_t read_r16_stack(uint8_t r16);
        uint16_t read_r16_mem(uint8_t r16);

        // write to register
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

        void interrupt_service_routine();

        // helper class for testing
        friend class CPUTester;
};

#undef REG_PAIR
