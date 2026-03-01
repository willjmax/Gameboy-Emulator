#include "timer.h"
#include <stdexcept>

void Timer::tick(uint8_t cycles) {
    for (int i = 0; i < cycles; i++) {
        uint16_t old_counter = internal_counter;

        if (!stopped) {
            internal_counter++;
        }

        uint16_t bit = tac_bit();
        bool enabled = tac_enabled();

        bool old_signal = (old_counter & (1 << bit)) && enabled;
        bool new_signal = (internal_counter & (1 << bit)) && enabled;

        if (old_signal & !new_signal) {
            if (tima == 0xFF) {
                tima = tma;
                interrupt.request_timer_interrupt();
            } else {
                tima++;
            }
        }
    }
}

uint8_t Timer::read_div() {
    return internal_counter >> 8;
}

uint8_t Timer::read_tac() {
    return Timer::tac;
}

uint8_t Timer::read_tima() {
    return Timer::tima;
}

uint8_t Timer::read_tma() {
    return Timer::tma;
}

void Timer::reset_div() {
    internal_counter = 0;
}

void Timer::write_tac(uint8_t data) {
    Timer::tac = data;
}

void Timer::write_tima(uint8_t data) {
    Timer::tima = data;
}

void Timer::write_tma(uint8_t data) {
    Timer::tma = data;
}

void Timer::enter_stop_mode() {
    Timer::stopped = true;
}

void Timer::exit_stop_mode() {
    Timer::stopped = false;
}

void Timer::enable_tac() {
    Timer::tac |= 0x04;
}

void Timer::disable_tac() {
    Timer::tac &= ~0x04;
}

bool Timer::tac_enabled() {
    return (Timer::tac & 0x04) == 0x04;
}

int Timer::tac_bit() {

    switch (Timer::tac & 0x03) {
        case 0b00: return 9;
        case 0b01: return 3;
        case 0b10: return 5;
        case 0b11: return 7;
        default: 
            throw std::runtime_error("tac_bit: impossible value");
    }

}
