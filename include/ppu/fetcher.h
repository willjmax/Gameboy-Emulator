#pragma once
#include <cstdint>
#include <optional>

#include "ppu/oam.h"

class PPU;

struct Pixel {
    uint8_t color_id;
    bool priority;
};

class PixelFIFO {
    public:
        void push(Pixel pixel) {
            if (count < 16) {
                buffer[tail] = pixel;
                tail = (tail + 1) % 16;
                count++;
            }
        }

        std::optional<Pixel> pop() {
            if (count == 0) return std::nullopt;
            Pixel pixel = buffer[head];
            head = (head + 1) % 16;
            count--;
            return pixel;
        }

        std::optional<Pixel> peak() {
            if (count == 0) return std::nullopt;
            return buffer[head];
        }

        void clear() { head = 0; tail = 0; count = 0;}
        uint8_t size() const { return count; }

    private:
        Pixel buffer[16];
        uint8_t head = 0;
        uint8_t tail = 0;
        uint8_t count = 0;

};

enum class BG_State : uint8_t {
    READ_TILE_ID     = 0,
    READ_FIRST_BYTE  = 1,
    READ_SECOND_BYTE = 2,
    PUSH_TO_FIFO     = 3,
};

enum class OBJ_State : uint8_t {
    GET_SPRITE_TILE = 0,
    GET_SPRITE_LOW  = 1,
    GET_SPRITE_HIGH = 2,
    MERGE_FIFO      = 3,
};

enum class FetcherMode : uint8_t {
    BACKGROUND = 0,
    WINDOW     = 1,
    OBJECT     = 2,
};

class PixelFetcher {
    public:
        PixelFetcher(PPU* parent_ppu) : ppu(parent_ppu) {};

        void reset(FetcherMode f_mode);
        void tick();
        std::optional<Pixel> select();
        FetcherMode fetcher_mode();

        void inc_window();
        void reset_window();
        void request_obj_mode(Sprite* sprite);

    private:
        FetcherMode mode = FetcherMode::BACKGROUND;
        BG_State bg_state = BG_State::READ_TILE_ID;
        OBJ_State obj_state = OBJ_State::GET_SPRITE_TILE;
        PixelFIFO BG_FIFO, OBJ_FIFO;
        PPU* ppu;

        uint16_t tile_id;
        uint16_t tile_index = 0;
        uint16_t window_count = -1;
        uint8_t ticks = 0;
        uint8_t byte1;
        uint8_t byte2;
        bool delay = true;

        // bg mode states
        void read_tile_id();
        void read_first_byte();
        void read_second_byte();
        void push_to_fifo();

        // obj mode states
        void get_sprite_tile();
        void get_sprite_low();
        void get_sprite_high();
        void merge_fifo();
        FetcherMode prev_mode;
        bool obj_requested;
        Sprite* oam_sprite;
};
