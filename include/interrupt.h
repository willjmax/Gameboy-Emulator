#include "memory.h"

class Interrupt {
    public:
        Interrupt(Bus& b) : 
            memory(b), 
            halted(false),
            ime(false),
            trigger_set_ime(false),
            delay_ime(false) {};

        uint16_t check_interrupts();
        bool is_halted();
        void halt_cpu();

        void set_ime_from_delay();
        void set_ime_delay();
        void set_ime();
        void clear_ime();
        bool ime_enabled();

        uint8_t read_interrupt_enable();
        uint8_t read_interrupt_flag();
        uint16_t interrupt_vector(uint8_t interrupt);

        void request_vblank_interrupt();
        void request_stat_interrupt();
        void request_timer_interrupt();
        void request_serial_interrupt();
        void request_joypad_interrupt();

        void clear_vblank_interrupt();
        void clear_stat_interrupt();
        void clear_timer_interrupt();
        void clear_serial_interrupt();
        void clear_joypad_interrupt();

    private:
        Bus& memory;
        bool halted;
        bool ime;
        bool trigger_set_ime;
        bool delay_ime;

        // interrupt enablers
        static constexpr uint16_t IF_REG    = 0xFF0F;
        static constexpr uint16_t IF_ENABLE = 0xFFFF;

        // interrupt vectors
        static constexpr uint16_t VBLANK = 0x0040;
        static constexpr uint16_t STAT   = 0x0048;
        static constexpr uint16_t TIMER  = 0x0050;
        static constexpr uint16_t SERIAL = 0x0058;
        static constexpr uint16_t JOYPAD = 0x0060;

        // for testing
        friend class CPUTester;
};
