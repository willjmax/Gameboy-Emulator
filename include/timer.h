#pragma once
#include <cstdint>
#include "interrupt.h"

class Timer {
    public:
        Timer(Interrupt& i) : 
            interrupt(i),
            stopped(false),
            internal_counter(0),
            tima(0),
            tac(0) {};

        void tick(uint8_t cycles);

        uint8_t read_div();
        uint8_t read_tima();
        uint8_t read_tma();
        uint8_t read_tac();

        void reset_div();
        void write_tima(uint8_t data);
        void write_tma(uint8_t data);
        void write_tac(uint8_t data);

        void enter_stop_mode();
        void exit_stop_mode();

        void enable_tac();
        void disable_tac();
        bool tac_enabled();
        int tac_bit();

    private:
        Interrupt& interrupt;
        bool stopped;
        uint16_t internal_counter;
        uint8_t tima;
        uint8_t tma;
        uint8_t tac;

        friend class CPUTester;
};
