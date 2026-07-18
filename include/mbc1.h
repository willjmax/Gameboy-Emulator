#include <cstdint>

class MBC1 {
    public:

        uint8_t read(uint16_t loc);
        void write(uint16_t loc, uint8_t data);

    private:

        static constexpr uint16_t ROM_BANK_X0_START = 0x0000;
        static constexpr uint16_t ROM_BANK_X0_END   = 0x3FFF;

        static constexpr uint16_t ROM_BANK_START = 0x4000;
        static constexpr uint16_t ROM_BANK_END   = 0x7FFF;

        static constexpr uint16_t RAM_BANK_START = 0xA000;
        static constexpr uint16_t RAM_BANK_END   = 0xBFFF;

};
