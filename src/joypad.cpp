#include "joypad.h"

uint8_t Joypad::read_joypad() {
    return reg;
}

void Joypad::write_joypad(uint8_t data) {
    reg = (reg & 0x0F) | (data & 0xF0);
}

void Joypad::press_button(Button button) {

    bool already_pressed = false;
     
    for (int i = 0; i < 4; i++) {
        bool x = (reg & (1 << i)) == 0;
        already_pressed = already_pressed || x;
    }

    switch (button) {

        case Button::UP:
            reg &= ~0x04;
            break;

        case Button::DOWN:
            reg &= ~0x08;
            break;

        case Button::LEFT:
            reg &= ~0x02;
            break;

        case Button::RIGHT:
            reg &= ~0x01;
            break;

        case Button::A:
            reg &= ~0x01;
            break;

        case Button::B:
            reg &= ~0x02;
            break;

        case Button::START:
            reg &= ~0x08;
            break;

        case Button::SELECT:
            reg &= ~0x04;
            break;
    }

    if (!already_pressed) {
        interrupt.request_joypad_interrupt();
    }

}

void Joypad::release_button(Button button) {

    switch (button) {

        case Button::UP:
            reg |= 0x04;
            break;

        case Button::DOWN:
            reg |= 0x08;
            break;

        case Button::LEFT:
            reg |= 0x02;
            break;

        case Button::RIGHT:
            reg |= 0x01;
            break;

        case Button::A:
            reg |= 0x01;
            break;

        case Button::B:
            reg |= 0x02;
            break;

        case Button::START:
            reg |= 0x08;
            break;

        case Button::SELECT:
            reg |= 0x04;
            break;
    }

}
