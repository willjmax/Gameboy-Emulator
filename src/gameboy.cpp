#include <SDL3/SDL_keycode.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <SDL3/SDL.h>
#include "gameboy.h"

#include <iostream>

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
            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                    handle_joypad_input(event.key.key, true);
                    break;

                case SDL_EVENT_KEY_UP:
                    handle_joypad_input(event.key.key, false);
                    break;

                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                default:
                    break;
            }
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

void GameBoy::handle_joypad_input(SDL_Keycode key, bool pressed) {

    void (Joypad::*action)(Button) = pressed ? &Joypad::press_button : &Joypad::release_button;

    switch (key) {

        case SDLK_W:
            (joypad.*action)(Button::UP);
            break;

        case SDLK_A:
            (joypad.*action)(Button::LEFT);
            break;

        case SDLK_S:
            (joypad.*action)(Button::DOWN);
            break;

        case SDLK_D:
            (joypad.*action)(Button::RIGHT);
            break;

        case SDLK_J:
            (joypad.*action)(Button::B);
            break;

        case SDLK_K:
            (joypad.*action)(Button::A);
            break;

        case SDLK_RETURN:
            (joypad.*action)(Button::START);
            break;

        case SDLK_RSHIFT:
            (joypad.*action)(Button::SELECT);
            break;

        default:
            break;
    }
}
