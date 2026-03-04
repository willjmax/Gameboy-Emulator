#include <cstdint>


class PPU {
    public:
        PPU() {};

    private:
        static constexpr uint8_t WIDTH  = 160;
        static constexpr uint8_t HEIGHT = 144;

        static constexpr uint16_t VRAM_START = 0x8000;
        static constexpr uint16_t VRAM_END   = 0x9FFF;
        static constexpr uint16_t OAM_START  = 0xFE00;
        static constexpr uint16_t OAM_END    = 0xFE9F;

        static constexpr uint16_t VRAM_SIZE = VRAM_END - VRAM_START + 1;
        static constexpr uint16_t OAM_SIZE = OAM_END - OAM_START + 1;

        // LCD addresses
        static constexpr uint16_t LCDC = 0xFF40;
        static constexpr uint16_t STAT = 0xFF41;
        static constexpr uint16_t SCY  = 0xFF42;
        static constexpr uint16_t SCX  = 0xFF43;
        static constexpr uint16_t LY   = 0xFF44;
        static constexpr uint16_t LYC  = 0xFF45;
        static constexpr uint16_t BGP  = 0xFF47;
        static constexpr uint16_t OBP0 = 0xFF48;
        static constexpr uint16_t OBP1 = 0xFF49;
        static constexpr uint16_t WY   = 0xFF4A;
        static constexpr uint16_t WX   = 0xFF4B;
        static constexpr uint16_t BCPS = 0xFF68;
        static constexpr uint16_t BCPD = 0xFF69;
        static constexpr uint16_t OCPS = 0xFF6A;
        static constexpr uint16_t OCPD = 0xFF6B;

        uint8_t vram[VRAM_SIZE];
        uint8_t oam[OAM_SIZE];
        uint8_t framebuffer[WIDTH * HEIGHT];
};
