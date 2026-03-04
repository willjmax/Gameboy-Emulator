#include "ppu_utils/tile.h"

Tile::Tile(std::array<uint8_t, 16> bytes) {

    int row;
    uint8_t byte1, byte2;
    uint8_t bit1, bit2;

    for (int i = 0; i < 16; i += 2) {
        row = i / 2; 
        byte1 = bytes[i];
        byte2 = bytes[i+1];

        for (int j = 0; j < 8; j++) {
            bit1 = (byte2 >> (7 - j)) & 0x01;
            bit2 = (byte1 >> (7 - j)) & 0x01;

            pixels[row][j] = (bit1 << 1) | bit2;
        }

    }

}

PixelMap Tile::pixel_map() {
    return pixels;
}
