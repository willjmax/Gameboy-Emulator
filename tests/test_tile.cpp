#include <array>
#include <catch2/catch_test_macros.hpp>

#include "ppu_utils/tile.h"

TEST_CASE("TILE", "[ppu]") {
    std::array<uint8_t, 16> bytes = { 
        0x3C, 0x7E, 0x42, 0x42,
        0x42, 0x42, 0x42, 0x42,
        0x7E, 0x5E, 0x7E, 0x0A,
        0x7C, 0x56, 0x38, 0x7C
    };

    std::array<std::array<uint8_t, 8>, 8> expected_tile = {{
        {0b00, 0b10, 0b11, 0b11, 0b11, 0b11, 0b10, 0b00},
        {0b00, 0b11, 0b00, 0b00, 0b00, 0b00, 0b11, 0b00},
        {0b00, 0b11, 0b00, 0b00, 0b00, 0b00, 0b11, 0b00},
        {0b00, 0b11, 0b00, 0b00, 0b00, 0b00, 0b11, 0b00},
        {0b00, 0b11, 0b01, 0b11, 0b11, 0b11, 0b11, 0b00},
        {0b00, 0b01, 0b01, 0b01, 0b11, 0b01, 0b11, 0b00},
        {0b00, 0b11, 0b01, 0b11, 0b01, 0b11, 0b10, 0b00},
        {0b00, 0b10, 0b11, 0b11, 0b11, 0b10, 0b00, 0b00}
    }};

    Tile tile(bytes);

    REQUIRE(tile.pixel_map() == expected_tile);
}
