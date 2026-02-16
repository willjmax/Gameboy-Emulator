#pragma once
#include "cpu.h"
#include "memory.h"

class GameBoy {
    public:
        GameBoy() : memory(), cpu(memory) {}

        void loadROM();
        void run();
        
    private:
        Bus memory;
        CPU cpu;
};

