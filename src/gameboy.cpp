#include <fstream>
#include <filesystem>

#include "gameboy.h"
#include <iostream>

void GameBoy::loadROM(std::string path) {
    std::ifstream file(path, std::ios::binary);
    std::uintmax_t size = std::filesystem::file_size(path);
    bus.loadROM(file, size);
}

void GameBoy::run() {
    running = true;
    while (running) {
        if (interrupt.is_halted()) {
            bus.tick(4);
        } else {
            cpu.step();
        }

        cpu.handle_interrupt();
        interrupt.set_ime_from_delay();
    }
}
