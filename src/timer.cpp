#include "memory.h"
#include "timer.h"
#include <stdexcept>

void Timer::tick(uint8_t cycles) {

    for (int i = 0; i < cycles; i++) {
        uint16_t old_counter = internal_counter;

        if (!stopped) {
            internal_counter++;
        }

        memory.update_div(internal_counter >> 8);
        uint16_t bit = tac_bit();
        bool enabled = tac_enabled();

        bool old_signal = (old_counter & (1 << bit)) && enabled;
        bool new_signal = (internal_counter & (1 << bit)) && enabled;

        if (old_signal & !new_signal) {
            uint8_t tima = memory.read(Timer::TIMA);
            uint8_t tma = memory.read(Timer::TMA);

            if (tima == 0xFF) {
                memory.write(Timer::TIMA, tma);
                memory.request_timer_interrupt();
            } else {
                memory.write(Timer::TIMA, tima+1);
            }

        }
    }
}

void Timer::reset_div() {
    internal_counter = 0;
    memory.write(Timer::DIV, 0x00);
}

void Timer::enter_stop_mode() {
    Timer::stopped = true;
}

void Timer::exit_stop_mode() {
    Timer::stopped = false;
}

void Timer::enable_tac() {
    uint8_t tac = memory.read(Timer::TAC);
    memory.write(Timer::TAC, tac | 0x04);
}

void Timer::disable_tac() {
    uint8_t tac = memory.read(Timer::TAC);
    memory.write(Timer::TAC, tac & ~0x04);
}

bool Timer::tac_enabled() {
    return (memory.read(Timer::TAC) & 0x04) == 0x04;
}

int Timer::tac_bit() {

    switch (memory.read(Timer::TAC) & 0x03) {
        case 0b00: return 9;
        case 0b01: return 3;
        case 0b10: return 5;
        case 0b11: return 7;
        default: 
            throw std::runtime_error("tac_bit: impossible value");
    }

}
