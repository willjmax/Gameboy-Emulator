#include "memory.h"

class Timer {
    public:
        Timer(Bus& b) : 
            memory(b), 
            stopped(false),
            internal_counter(0) {}

        void tick(uint8_t cycles);
        void reset_div();

        void enter_stop_mode();
        void exit_stop_mode();

        void enable_tac();
        void disable_tac();
        bool tac_enabled();
        int tac_bit();

    private:
        Bus& memory;
        bool stopped;
        uint16_t internal_counter;

        static constexpr uint16_t DIV  = 0xFF04;
        static constexpr uint16_t TIMA = 0xFF05;
        static constexpr uint16_t TMA  = 0xFF06;
        static constexpr uint16_t TAC  = 0xFF07;

        friend class CPUTester;
};
