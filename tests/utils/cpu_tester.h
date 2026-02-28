#pragma once

#include <nlohmann/json.hpp>
#include "cpu.h"

using json = nlohmann::json;

class CPUTester {
    public:
        CPUTester() : bus(), cpu(bus) {};
        void setup_sm83(json initial);
        bool verify(json final);

        uint16_t get_pc() { return cpu.pc; };
        uint16_t get_sp() { return cpu.sp; };

        void step() { cpu.step(); };

        uint8_t get_a() { return cpu.a; };
        uint8_t get_b() { return cpu.b; };
        uint8_t get_c() { return cpu.c; };
        uint8_t get_d() { return cpu.d; };
        uint8_t get_e() { return cpu.e; };
        uint8_t get_f() { return cpu.f; };
        uint8_t get_h() { return cpu.h; };
        uint8_t get_l() { return cpu.l; };
        uint8_t get_ime() { return cpu.interrupts.ime; };

        uint8_t get_memory(uint16_t loc) {
            return cpu.memory.read(loc);
        }

    private:
        Bus bus;
        CPU cpu;

};
