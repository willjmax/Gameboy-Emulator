#include "apu.h"

APU::APU() {
    for (int i = 0; i < BUS_SIZE; ++i) {
        bus[i] = 0;
        masks[i] = 0;
        write_handlers[i] = &APU::write_default;
        read_handlers[i] = &APU::read_default;
    }

    set_masks();
    set_write_handlers();
}

uint8_t APU::read_default(uint16_t loc) {
    return bus[loc] | masks[loc];
}

void APU::write_default(uint16_t loc, uint8_t data) {
    bus[loc] = data;
}

uint8_t APU::read(uint16_t loc) {
    uint16_t shifted_loc = loc - BUS_START;
    ReadHandler handler = read_handlers[shifted_loc];
    return (this->*handler)(shifted_loc);
}

void APU::write(uint16_t loc, uint8_t data) {

    if (read_only && !read_only_exempt(loc)) {
        return;
    }

    uint16_t shifted_loc = loc - BUS_START;
    WriteHandler handler = write_handlers[shifted_loc];
    (this->*handler)(shifted_loc, data);

}

void APU::tick(uint8_t cycles) {

}

void APU::set_masks() {
    // NR10-NR15
    masks[0xFF10 - BUS_START] = 0x80;
    masks[0xFF11 - BUS_START] = 0x3F;
    masks[0xFF12 - BUS_START] = 0x00;
    masks[0xFF13 - BUS_START] = 0xFF;
    masks[0xFF14 - BUS_START] = 0xBF;

    // NR20-NR25
    masks[0xFF15 - BUS_START] = 0xFF;
    masks[0xFF16 - BUS_START] = 0x3F;
    masks[0xFF17 - BUS_START] = 0x00;
    masks[0xFF18 - BUS_START] = 0xFF;
    masks[0xFF19 - BUS_START] = 0xBF;

    // NR30-NR35
    masks[0xFF1A - BUS_START] = 0x7F;
    masks[0xFF1B - BUS_START] = 0xFF;
    masks[0xFF1C - BUS_START] = 0x9F;
    masks[0xFF1D - BUS_START] = 0xFF;
    masks[0xFF1E - BUS_START] = 0xBF;

    // NR40-45
    masks[0xFF1F - BUS_START] = 0xFF;
    masks[0xFF20 - BUS_START] = 0xFF;
    masks[0xFF21 - BUS_START] = 0x00;
    masks[0xFF22 - BUS_START] = 0x00;
    masks[0xFF23 - BUS_START] = 0xBF;

    // NR50-NR52
    masks[0xFF24 - BUS_START] = 0x00;
    masks[0xFF25 - BUS_START] = 0x00;
    masks[0xFF26 - BUS_START] = 0x70;

    // UNUSED BEFORE WAVE RAM
    for (uint16_t i = 0xFF27; i < 0xFF30; i++) {
        masks[i - BUS_START] = 0xFF;
    }
}

void APU::write_NR52(uint16_t loc, uint8_t data) {

    if ((data & 0x80) == 0) {

        for (uint16_t i = BUS_START; i < NR52; i++) {
            bus[i - BUS_START] = 0;
        }

        read_only = true;
    } else {
        read_only = false;
    }

    uint16_t current = bus[loc - BUS_START];
    uint16_t updated = (data & 0x80) | (current & ~0x80);

    bus[loc - BUS_START] = updated;
}

void APU::set_write_handlers() {
    write_handlers[0xFF26 - BUS_START] = &APU::write_NR52;
}

bool APU::read_only_exempt(uint16_t loc) {

    if (loc == NR52) {
        return true;
    }

    if (loc >= WAVE_START) {
        return true;
    }

    return false;
}
