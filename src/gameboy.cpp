#include <fstream>
#include <filesystem>
#include <iostream>
#include <SDL3/SDL.h>
#include "gameboy.h"

void GameBoy::loadROM(std::string path) {
    std::ifstream file(path, std::ios::binary);
    std::uintmax_t size = std::filesystem::file_size(path);
    bus.loadROM(file, size);
}

void GameBoy::run() {
    running = true;

    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        if (interrupt.is_halted()) {
            bus.tick(4);
        } else {
            cpu.step();
        }

        cpu.handle_interrupt();
        interrupt.set_ime_from_delay();

        if (ppu.ready()) {
            display.update(ppu.get_framebuffer());
            ppu.frame_rendered();
        }

    }
}
