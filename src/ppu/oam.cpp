#include <ppu/oam.h>

Sprite::Sprite() {
    _priority = false;
    _y_flip = false;
    _x_flip = false;
    _palette = PPU_REG::OBP1;
}

Sprite::Sprite(uint8_t y_pos, uint8_t x_pos, uint8_t tile_index, uint8_t attributes) :
    _y_pos(y_pos), _x_pos(x_pos), _tile_index(tile_index) {

    _priority = (attributes & 128) == 128;
    _y_flip = (attributes & 64) == 64;
    _x_flip = (attributes & 32) == 32;

    if ((attributes & 16) == 16) {
        _palette = PPU_REG::OBP1;
    } else {
        _palette = PPU_REG::OBP0;
    }
}

bool Sprite::on_scanline(int ly, int obj_size) {
    if (_y_pos > ly + 16) {
        return false;
    }

    if (_y_pos + obj_size <= ly + 16) {
        return false;
    }

    return true;
}
