#pragma once
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

        bool getZ();
        bool getN();
        bool getH();
        bool getC();

        void setZ(bool bit);
        void setN(bool bit);
        void setH(bool bit);
        void setC(bool bit);

    private:
        Bus& memory;
        REG_PAIR(a, f, af);
        REG_PAIR(b, c, bc);
        REG_PAIR(d, e, de);
        REG_PAIR(h, l, hl);
        uint16_t pc;
        uint16_t sp;
};

#undef REG_PAIR
