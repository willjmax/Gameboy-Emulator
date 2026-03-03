#pragma once
#include <cstdint>

class APU {
    public:
        APU();
        uint8_t read(uint16_t loc);
        void write(uint16_t loc, uint8_t data);
        void tick(uint8_t cycles);

    private:
        using ReadHandler = uint8_t (APU::*)(uint16_t);
        using WriteHandler = void (APU::*)(uint16_t, uint8_t);

        static constexpr uint16_t BUS_START  = 0xFF10;
        static constexpr uint16_t NR52       = 0xFF26;
        static constexpr uint16_t WAVE_START = 0xFF30;
        static constexpr uint16_t BUS_END    = 0xFF3F;
        static constexpr uint16_t BUS_SIZE   = BUS_END - BUS_START + 1;

        uint8_t bus[BUS_SIZE];
        uint8_t masks[BUS_SIZE];
        ReadHandler read_handlers[BUS_SIZE];
        WriteHandler write_handlers[BUS_SIZE];

        uint8_t read_default(uint16_t loc);
        void write_default(uint16_t loc, uint8_t data);

        bool read_only;
        bool read_only_exempt(uint16_t loc);

        void set_masks();
        void set_write_handlers();

        void write_NR52(uint16_t loc, uint8_t data);
};
