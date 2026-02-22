#pragma once
#include <string>
#include "cpu.h"
#include "memory.h"

class GameBoy {
    public:
        GameBoy();

        Bus& getBus() { return memory; };
        CPU& getCPU() { return cpu; };

        void loadROM(std::string path);
        void run();
        
    private:
        Bus memory;
        CPU cpu;
        bool running;
};
