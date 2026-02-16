#include <cstdint>

namespace MemoryMap {
    constexpr uint16_t ROM_FIXED_START = 0x0000; // Bank 00
    constexpr uint16_t ROM_SWAP_START  = 0x4000; // Bank 01-NN
    constexpr uint16_t VRAM_START      = 0x8000;
    constexpr uint16_t SRAM_START      = 0xA000; // External Cartridge RAM
    constexpr uint16_t WRAM1_START     = 0xC000;
    constexpr uint16_t WRAM2_START     = 0xD000;
    constexpr uint16_t ECHO_START      = 0xE000;
    constexpr uint16_t OAM_START       = 0xFE00;
    constexpr uint16_t NOT_USABLE      = 0xFEA0;
    constexpr uint16_t IO_START        = 0xFF00;
    constexpr uint16_t HRAM_START      = 0xFF80;
    constexpr uint16_t IE_REG          = 0xFFFF; // Interrupt Enable Register
}
