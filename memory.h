#pragma once
#include <cstdint>

class Bus {
    public:
        uint8_t read();
        void write();

    private:
        uint8_t memory[0xFFFF];
};
