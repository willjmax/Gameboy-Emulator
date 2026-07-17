#include <cstdint>

#include "interrupt.h"

enum class Button {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    A,
    B,
    START,
    SELECT,
};


class Joypad {
    public:
        Joypad(Interrupt& interrupt) : interrupt(interrupt) {};
        uint8_t read_joypad();
        void write_joypad(uint8_t data);

        void press_button(Button button);
        void release_button(Button button);

    private:
        Interrupt& interrupt;
        uint8_t reg = 0xCF;
};
