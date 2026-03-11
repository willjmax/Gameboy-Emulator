#include <array>
#include <cstdint>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include "display.h"

Display::Display() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_CreateWindowAndRenderer("Gameboy", WIDTH * SCALE, HEIGHT * SCALE, 0, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

}

Display::~Display() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::update(std::array<uint8_t, WIDTH*HEIGHT> ppu_framebuffer) {
    static constexpr int N_PIXELS = WIDTH * HEIGHT;
    uint32_t pixels[N_PIXELS];

    for (int i = 0; i < N_PIXELS; i++) {
        pixels[i] = palette[ppu_framebuffer[i]];
    }

    int pitch = WIDTH * sizeof(uint32_t);

    SDL_UpdateTexture(texture, NULL, pixels, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
