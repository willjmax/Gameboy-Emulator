#include "interrupt.h"

uint16_t Interrupt::check_interrupts() {
    uint8_t flags = read_interrupt_flag();
    uint8_t enabled = read_interrupt_enable();

    uint8_t interrupts = flags & enabled;

    if (interrupts & 0x01) {
        halted = false;

        if (ime) {
            ime = false;
            clear_vblank_interrupt();
            return Interrupt::VBLANK;
        }
    }

    if (interrupts & 0x02) {
        halted = false;

        if (ime) {
            ime = false;
            clear_stat_interrupt();
            return Interrupt::STAT;
        }
    }

    if (interrupts & 0x04) {
        halted = false;

        if (ime) {
            ime = false;
            clear_timer_interrupt();
            return Interrupt::TIMER;
        }
    }

    if (interrupts & 0x08) {
        halted = false;
        
        if (ime) {
            ime = false;
            clear_serial_interrupt();
            return Interrupt::SERIAL;
        }
    }

    if (interrupts & 0x10) {
        halted = false;

        if (ime) {
            ime = false;
            clear_joypad_interrupt();
            return Interrupt::JOYPAD;
        }
    }

    return 0;
}

uint8_t Interrupt::read_interrupt_enable() {
    return memory.read(Interrupt::IF_ENABLE);
}

uint8_t Interrupt::read_interrupt_flag() {
    return memory.read(Interrupt::IF_REG);
}

void Interrupt::request_vblank_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg | 0x01);
}

void Interrupt::request_stat_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg | 0x02);
}

void Interrupt::request_timer_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg | 0x04);
}

void Interrupt::request_serial_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg | 0x08);
}

void Interrupt::request_joypad_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg | 0x0F);
}

void Interrupt::clear_vblank_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg & ~0x01);
}

void Interrupt::clear_stat_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg & ~0x02);
}

void Interrupt::clear_timer_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg & ~0x04);
}

void Interrupt::clear_serial_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg & ~0x08);
}

void Interrupt::clear_joypad_interrupt() {
    uint8_t if_reg = memory.read(Interrupt::IF_REG);
    memory.write(Interrupt::IF_REG, if_reg & ~0x0F);
}

void Interrupt::set_ime_delay() {
    delay_ime = true;
}

void Interrupt::set_ime() {
    ime = true;
}

void Interrupt::clear_ime() {
    ime = false;
    trigger_set_ime = false;
    delay_ime = false;
}

void Interrupt::set_ime_from_delay() {

    if (trigger_set_ime) {
        ime = true;
        trigger_set_ime = false;
    }

    if (delay_ime) { 
        delay_ime = false;
        trigger_set_ime = true;
    }

}

bool Interrupt::is_halted() {
    return halted;
}

void Interrupt::halt_cpu() {
    halted = true;
}
