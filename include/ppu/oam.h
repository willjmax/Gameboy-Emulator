#pragma once
#include <cstdint>
#include <ppu/registers.h>

class Sprite {
    public:
        Sprite();
        Sprite(uint8_t y_pos, uint8_t x_pos, uint8_t tile_index, uint8_t attributes);

        uint8_t y_pos() const { return _y_pos; };
        uint8_t x_pos() const { return _x_pos; };
        uint8_t tile_index() const { return _tile_index; };
        bool priority() const { return _priority; };
        bool y_flip() const { return _y_flip; };
        bool x_flip() const { return _x_flip; };
        PPU_REG palette() const { return _palette; };

        bool on_scanline(int ly, int obj_size);

        bool fetched = false;

    private:
        uint8_t _y_pos;
        uint8_t _x_pos;
        uint8_t _tile_index;

        bool _priority;
        bool _y_flip;
        bool _x_flip;
        PPU_REG _palette;
};
