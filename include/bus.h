#pragma once
#include <cstdint>
#include <fstream>

#include "apu.h"
#include "ppu.h"
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
        static constexpr uint16_t ROM_SWAP_START  = 0x4000; // Bank 01-NN
        static constexpr uint16_t VRAM_START      = 0x8000;
        static constexpr uint16_t SRAM_START      = 0xA000; // External Cartridge RAM
        static constexpr uint16_t WRAM1_START     = 0xC000;
        static constexpr uint16_t WRAM2_START     = 0xD000;
        static constexpr uint16_t ECHO_START      = 0xE000;
        static constexpr uint16_t OAM_START       = 0xFE00;
        static constexpr uint16_t NOT_USABLE      = 0xFEA0;
        static constexpr uint16_t IO_START        = 0xFF00;
        static constexpr uint16_t APU_START       = 0xFF10;
        static constexpr uint16_t APU_END         = 0xFF3F;
        static constexpr uint16_t HRAM_START      = 0xFF80;

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
