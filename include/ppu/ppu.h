#include <array>
#include <cstdint>
#include <vector>
#include "interrupt.h"
#include "ppu/fetcher.h"
#include "ppu/oam.h"

#include <fstream>

enum class PPU_Mode : uint8_t {
    HBLANK   = 0,
    VBLANK   = 1,
    OAM_SCAN = 2,
    DRAWING  = 3
};


class PPU {
    private:
        Interrupt& interrupt;
        int dots = 0;
        PPU_Mode mode = PPU_Mode::OAM_SCAN;
        bool frame_ready = false;
        int x_coord = 0;
        int scx_cnt = 0;

        static constexpr uint16_t VRAM_START = 0x8000;
        static constexpr uint16_t VRAM_END   = 0x9FFF;
        static constexpr uint16_t VRAM_SIZE = VRAM_END - VRAM_START + 1;

        static constexpr uint16_t OAM_START  = 0xFE00;
        static constexpr uint16_t OAM_END    = 0xFE9F;
        static constexpr uint16_t OAM_SIZE = OAM_END - OAM_START + 1;

        static constexpr uint16_t REG_START = 0xFF40;
        static constexpr uint16_t REG_END   = 0xFF4B;
        static constexpr uint16_t REG_SIZE  = REG_END - REG_START + 1;

        static constexpr uint16_t MAP_AREA_0_START = 0x9800 - VRAM_START;
        static constexpr uint16_t MAP_AREA_1_START = 0x9C00 - VRAM_START;

        static constexpr uint16_t DATA_AREA_0_START = 0x9000 - VRAM_START;
        static constexpr uint16_t DATA_AREA_1_START = 0x8000 - VRAM_START;

        static constexpr uint8_t WIDTH  = 160;
        static constexpr uint8_t HEIGHT = 144;

        std::array<uint8_t, VRAM_SIZE> vram;
        std::array<uint8_t, OAM_SIZE> oam;
        std::array<uint8_t, REG_SIZE> registers;
        void initialize_registers();

        friend class PixelFetcher;

        PixelFetcher fetcher;
        std::vector<Sprite> sprite_buffer;
        Sprite fetch_sprite(uint16_t offset);

        using ReadHandler = uint8_t (PPU::*)(uint16_t);
        using WriteHandler = void (PPU::*)(uint16_t, uint8_t);

        // interal read/write
        uint8_t read_register(PPU_REG reg);
        void write_register(PPU_REG reg, uint8_t data);
        
        std::array<ReadHandler, REG_SIZE> read_reg_handlers;
        std::array<WriteHandler, REG_SIZE> write_reg_handlers;
        uint8_t read_reg_default(uint16_t loc);
        void write_reg_default(uint16_t loc, uint8_t data);
        void set_write_handlers();
        void set_read_handlers();
        void write_LY(uint16_t loc, uint8_t data);
        void inc_LY();
        void reset_LY();
        void compare();

        // LCDC bits
        bool lcd_ppu_enabled();
        uint16_t window_tile_map_area();
        bool window_enabled();
        uint16_t tile_data_area();
        uint16_t bg_tile_map_area();
        int obj_size();
        bool obj_enable();
        bool bg_window_enabled();
        bool signed_mode();

        // STAT bits
        uint8_t read_stat(uint16_t loc);
        void write_stat(uint16_t loc, uint8_t data);

        // framebuffer
        using FrameBuffer = std::array<uint8_t, WIDTH*HEIGHT>;
        FrameBuffer framebuffer;
        void write_to_framebuffer(int x, int y, uint8_t pixel);

        // modes
        void mode_0_hblank();
        void mode_1_vblank(); 
        void mode_2_oam_scan();
        void mode_3_drawing();
        bool sprite_fetched = false;

        std::optional<Sprite> sprite_on_column();
        uint8_t color_id_lookup(Pixel pixel);

    public:
        PPU(Interrupt& i);

        uint8_t read_vram(uint16_t loc);
        uint8_t read_oam(uint16_t loc);
        uint8_t read_register(uint16_t loc);

        void write_vram(uint16_t loc, uint8_t data);
        void write_oam(uint16_t loc, uint8_t data);
        void write_register(uint16_t loc, uint8_t data);

        void tick(uint8_t cycles);

        FrameBuffer get_framebuffer() const { return framebuffer; };
        bool ready() const { return frame_ready; };
        void frame_rendered() { frame_ready = false; };
};

template<size_t N>
void dump_vram(const std::array<uint8_t, N> vram, const int size) {
    std::ofstream outFile("vram.dump", std::ios::out | std::ios::binary);

    outFile.write(reinterpret_cast<const char*>(vram.data()), size);
    outFile.close();
}
