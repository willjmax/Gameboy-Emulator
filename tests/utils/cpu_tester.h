#pragma once

#include <nlohmann/json.hpp>
#include "bus.h"
#include "cpu.h"
#include "timer.h"
#include "interrupt.h"

using json = nlohmann::json;

class CPUTester {
    public:
        CPUTester() :
            interrupt(), timer(interrupt),
            apu(), ppu(),
            bus(timer, interrupt, apu, ppu),
            cpu(bus, timer, interrupt) {};

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
        uint8_t get_ime() { return cpu.interrupt.ime; };

        uint8_t get_memory(uint16_t loc) {
            return cpu.bus.read(loc);
        }

    private:
        Interrupt interrupt;
        Timer timer;
        APU apu;
        PPU ppu;
        Bus bus;
        CPU cpu;

};
