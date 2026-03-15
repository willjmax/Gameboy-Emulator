#pragma once
#include <cstdint>
#include <fstream>

#include "apu.h"
#include "ppu/ppu.h"
#include "timer.h"
#include "interrupt.h"

class Bus {
    public:
        Bus(Timer& t, Interrupt& i, APU& apu, PPU& ppu) :
            timer(t), interrupt(i),
            apu(apu), ppu(ppu) {};

        uint8_t read(uint16_t loc);
        void write(uint16_t loc, uint8_t byte);
        void tick(int cycles);

        void loadROM(std::ifstream& file, uintmax_t size);

        void update_div(uint8_t div);

    private:
        uint8_t memory[0xFFFF];
        Timer& timer;
        Interrupt& interrupt;
        APU& apu;
        PPU& ppu;

        static constexpr uint16_t ROM_FIXED_START = 0x0000; // Bank 00

        // APU
        static constexpr uint16_t APU_START       = 0xFF10;
        static constexpr uint16_t APU_END         = 0xFF3F;

        // PPU
        static constexpr uint16_t VRAM_START = 0x8000;
        static constexpr uint16_t VRAM_END   = 0x9FFF;
        static constexpr uint16_t OAM_START  = 0xFE00;
        static constexpr uint16_t OAM_END    = 0xFE9F;
        static constexpr uint16_t PPU_START  = 0xFF40;
        static constexpr uint16_t PPU_END    = 0xFF4B;

        // Timer addresses
        static constexpr uint16_t DIV  = 0xFF04;
        static constexpr uint16_t TIMA = 0xFF05;
        static constexpr uint16_t TMA  = 0xFF06;
        static constexpr uint16_t TAC  = 0xFF07;

        // Interrupt addresses
        static constexpr uint16_t IF_REG    = 0xFF0F;
        static constexpr uint16_t IF_ENABLE = 0xFFFF;

        // blargg
        static constexpr uint16_t TERMINAL = 0xFF01;
};
