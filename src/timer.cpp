#include "memory.h"
#include "timer.h"
#include <stdexcept>

void Timer::tick(uint8_t cycles) {

    if (!stopped) {
        internal_counter += cycles;
        memory.update_div(internal_counter >> 8);
    }

    tima_counter += cycles;
    uint16_t tima_freq = tac_clock();
    uint16_t tima_enabled = tac_enabled();

    if (tima_counter >= tima_freq && tima_enabled) {

        tima_counter -= tima_freq;
        uint8_t tima = memory.read(Timer::TIMA);

        if (tima == 0xFF) {
            uint8_t tma = memory.read(Timer::TMA);
            memory.write(Timer::TIMA, tma);
            memory.request_timer_interrupt();
        } else {
            memory.write(Timer::TIMA, tima + 1);
        }

    }

}

void Timer::reset_div() {
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

uint16_t Timer::tac_clock() {

    switch (memory.read(Timer::TAC) & 0x03) {
        case 0b00: return 1028;
        case 0b01: return 16;
        case 0b10: return 64;
        case 0b11: return 256;
        //case 0b00: return 256;
        //case 0b01: return 4;
        //case 0b10: return 16;
        //case 0b11: return 64;
        default: 
            throw std::runtime_error("tac_clock: impossible value");
    }

}
