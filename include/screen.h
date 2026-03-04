#include <SDL3/SDL.h>

class Screen {
    public:
        Screen();
        ~Screen();

    private:

        static constexpr uint8_t WIDTH  = 160;
        static constexpr uint8_t HEIGHT = 144;

        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
};
