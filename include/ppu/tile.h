#include <array>
#include <cstdint>

using PixelMap = std::array<std::array<uint8_t, 8>, 8>;

class Tile {
    public:
        Tile(std::array<uint8_t, 16>);
        PixelMap pixel_map();

    private:
        PixelMap pixels;

};
