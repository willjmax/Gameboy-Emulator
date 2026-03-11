//#include "ppu/ppu.h"
//
//using FrameBuffer = std::array<uint8_t, PPU::HEIGHT * PPU::WIDTH>;
//
//FrameBuffer PPU::build_framebuffer() {
//    FrameBuffer framebuffer;
//
//    for (int row = 0; row < HEIGHT; row++) {
//        for (int col = 0; col < WIDTH; col++) {
//            framebuffer[(row * WIDTH) + col] = get_pixel(row, col);
//        }
//    }
//
//    return framebuffer;
//}
//
//Tile PPU::fetch_tile(uint16_t loc) {
//    std::array<uint8_t, 16> bytes;
//    for (int i = 0; i < 16; i++) {
//        bytes[i] = vram[loc + i];
//    }
//
//    return Tile(bytes);
//}
//
//uint8_t PPU::get_pixel(int x, int y) {
//    uint8_t mapX = x + registers[SCX] % 256;
//    uint8_t mapY = y + registers[SCY] % 256;
//
//    int tileCol = mapX / 8;
//    int tileRow = mapY / 8;
//    uint16_t mapAddr = bg_tile_map_area().first + (tileRow * 32) + tileCol;
//
//    Tile tile = fetch_tile(mapAddr - VRAM_START);
//    return tile.pixel_map()[x % 8][y % 8];
//}
