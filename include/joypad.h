#include <cstdint>

#include "interrupt.h"

class Joypad {
    public:
        Joypad(Interrupt& interrupt) : interrupt(interrupt) {};
        uint8_t read_joypad();

    private:
        Interrupt& interrupt;
        uint8_t reg = 0xCF;
};
