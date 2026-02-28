#pragma once
#include <cstdint>
#include <fstream>

class Bus {
    public:
        uint8_t read(uint16_t loc);
        void write(uint16_t loc, uint8_t byte);

        void loadROM(std::ifstream& file, uintmax_t size);

        void update_div(uint8_t div);

    private:
        uint8_t memory[0xFFFF];

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
        static constexpr uint16_t HRAM_START      = 0xFF80;

        // divider register, must reset upon write
        static constexpr uint16_t DIV  = 0xFF04;

        // first 3 bits must always be 1 
        static constexpr uint16_t IF_REG    = 0xFF0F;
                                                            
        // blargg
        static constexpr uint16_t TERMINAL = 0xFF01;
};
