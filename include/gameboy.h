#pragma once
#include <string>

#include "bus.h"
#include "cpu.h"
#include "interrupt.h"
#include "timer.h"

class GameBoy {
    public:
        GameBoy() :
            interrupt(),
            timer(interrupt),
            apu(),
            ppu(),
            bus(timer, interrupt, apu, ppu),
            cpu(bus, timer, interrupt),
            running(false) {};

        void loadROM(std::string path);
        void run();
        
    private:
        Interrupt interrupt;
        Timer timer;
        APU apu;
        PPU ppu;
        Bus bus;
        CPU cpu;

        bool running;
};
