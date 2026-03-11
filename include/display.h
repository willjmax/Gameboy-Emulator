#include <array>
#include <SDL3/SDL.h>

class Display {
    public:
        Display();
        ~Display();

        void update(std::array<uint8_t, 160*144> ppu_framebuffer);

    private:

        const uint32_t palette[4] = {
            0xFFFFFFFF,
            0xAAAAAAAA,
            0x55555555,
            0x00000000
        };

        static constexpr uint8_t WIDTH  = 160;
        static constexpr uint8_t HEIGHT = 144;
        static constexpr uint8_t SCALE = 4;

        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
};
